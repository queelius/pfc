// codec_bindings.cpp - Codec bindings for PFC Python

#include "pfc_python.hpp"
#include <pfc/codecs.hpp>
#include <functional>
#include <unordered_map>
#include <string>

namespace pfc_python {

// Codec wrapper for Python - provides a uniform interface for all codecs
class PyCodec {
public:
    using EncodeFunc = std::function<void(uint64_t, pfc::BitWriter&)>;
    using DecodeFunc = std::function<uint64_t(pfc::BitReader&)>;

    PyCodec() = default;

    PyCodec(std::string name, EncodeFunc encode, DecodeFunc decode)
        : name_(std::move(name)), encode_(std::move(encode)), decode_(std::move(decode)) {}

    void encode(uint64_t value, PyBitWriter& writer) const {
        encode_(value, writer.get_writer());
    }

    uint64_t decode(PyBitReader& reader) const {
        return decode_(reader.get_reader());
    }

    const std::string& name() const { return name_; }

private:
    std::string name_;
    EncodeFunc encode_;
    DecodeFunc decode_;
};

// Codec registry singleton - maps names to codec instances
class CodecRegistry {
public:
    static CodecRegistry& instance() {
        static CodecRegistry registry;
        return registry;
    }

    void register_codec(const std::string& name, PyCodec codec) {
        codecs_.emplace(name, std::move(codec));
    }

    const PyCodec& get(const std::string& name) const {
        auto it = codecs_.find(name);
        if (it == codecs_.end()) {
            throw std::invalid_argument("Unknown codec: " + name);
        }
        return it->second;
    }

    std::vector<std::string> list_codecs() const {
        std::vector<std::string> names;
        names.reserve(codecs_.size());
        for (const auto& [name, _] : codecs_) {
            names.push_back(name);
        }
        return names;
    }

private:
    CodecRegistry() = default;
    std::unordered_map<std::string, PyCodec> codecs_;
};

// Template helper to create codec encode/decode functions
template<typename Codec>
PyCodec make_codec(const std::string& name) {
    return PyCodec(
        name,
        [](uint64_t value, pfc::BitWriter& w) {
            Codec::template encode<uint64_t>(value, w);
        },
        [](pfc::BitReader& r) -> uint64_t {
            return Codec::template decode<uint64_t>(r);
        }
    );
}

// Factory for parameterized Rice codec
PyCodec make_rice_codec(unsigned k) {
    return PyCodec(
        "rice_" + std::to_string(k),
        [k](uint64_t value, pfc::BitWriter& w) {
            // Rice encoding: quotient in unary, remainder in binary
            uint64_t mask = (1ULL << k) - 1;
            uint64_t quotient = value >> k;
            uint64_t remainder = value & mask;

            // Write quotient in unary
            for (uint64_t i = 0; i < quotient; ++i) {
                w.write(false);
            }
            w.write(true);

            // Write remainder in binary (LSB first)
            for (unsigned i = 0; i < k; ++i) {
                w.write((remainder >> i) & 1);
            }
        },
        [k](pfc::BitReader& r) -> uint64_t {
            // Decode quotient
            uint64_t quotient = 0;
            while (!r.read()) {
                ++quotient;
            }

            // Decode remainder
            uint64_t remainder = 0;
            for (unsigned i = 0; i < k; ++i) {
                remainder |= static_cast<uint64_t>(r.read()) << i;
            }

            return (quotient << k) | remainder;
        }
    );
}

// Factory for parameterized ExpGolomb codec
PyCodec make_exp_golomb_codec(unsigned order) {
    return PyCodec(
        "exp_golomb_" + std::to_string(order),
        [order](uint64_t value, pfc::BitWriter& w) {
            value += (1ULL << order);  // Add 2^order offset
            int len = 0;
            uint64_t tmp = value;
            while (tmp > 0) {
                ++len;
                tmp >>= 1;
            }

            // Write len-1 zeros
            for (int i = 0; i < len - 1; ++i) {
                w.write(false);
            }

            // Write value in binary (MSB to LSB)
            for (int i = len - 1; i >= 0; --i) {
                w.write((value >> i) & 1);
            }
        },
        [order](pfc::BitReader& r) -> uint64_t {
            int zeros = 0;
            while (!r.read()) {
                ++zeros;
            }

            uint64_t value = 1;
            for (int i = 0; i < zeros; ++i) {
                value = (value << 1) | r.read();
            }

            return value - (1ULL << order);
        }
    );
}

// Factory for fixed-width codec
PyCodec make_fixed_codec(unsigned width) {
    return PyCodec(
        "fixed_" + std::to_string(width),
        [width](uint64_t value, pfc::BitWriter& w) {
            w.write_bits(value, width);
        },
        [width](pfc::BitReader& r) -> uint64_t {
            return r.read_bits(width);
        }
    );
}

// Signed integer wrapper using zigzag encoding
class SignedCodec {
public:
    SignedCodec() = default;
    explicit SignedCodec(PyCodec base) : base_(std::move(base)) {}

    void encode(int64_t value, PyBitWriter& writer) const {
        // Zigzag encoding: map signed to unsigned
        uint64_t zigzag = static_cast<uint64_t>((value << 1) ^ (value >> 63));
        base_.encode(zigzag, writer);
    }

    int64_t decode(PyBitReader& reader) const {
        uint64_t zigzag = base_.decode(reader);
        // Reverse zigzag
        return static_cast<int64_t>((zigzag >> 1) ^ -(static_cast<int64_t>(zigzag) & 1));
    }

private:
    PyCodec base_;
};

// High-level compress/decompress API
py::bytes compress(const std::vector<uint64_t>& data, const std::string& codec_name) {
    const auto& codec = CodecRegistry::instance().get(codec_name);

    // Estimate buffer size (generous)
    std::vector<uint8_t> buffer(data.size() * 10 + 16);
    pfc::BitWriter writer(buffer.data());

    // Write count first using VByte-style encoding
    size_t count = data.size();
    while (count >= 128) {
        writer.write_bits((count & 0x7F) | 0x80, 8);
        count >>= 7;
    }
    writer.write_bits(count, 8);

    // Create temporary PyBitWriter wrapper around the real writer
    // (We need to work directly with the BitWriter here)
    for (uint64_t value : data) {
        // Encode directly using the codec's internal functions
        if (codec_name == "elias_gamma") {
            pfc::codecs::EliasGamma::encode<uint64_t>(value, writer);
        } else if (codec_name == "elias_delta") {
            pfc::codecs::EliasDelta::encode<uint64_t>(value, writer);
        } else if (codec_name == "fibonacci") {
            pfc::codecs::Fibonacci::encode<uint64_t>(value, writer);
        } else if (codec_name == "unary") {
            pfc::codecs::Unary::encode<uint64_t>(value, writer);
        } else if (codec_name.find("rice_") == 0) {
            unsigned k = std::stoi(codec_name.substr(5));
            uint64_t mask = (1ULL << k) - 1;
            uint64_t quotient = value >> k;
            uint64_t remainder = value & mask;
            for (uint64_t i = 0; i < quotient; ++i) writer.write(false);
            writer.write(true);
            for (unsigned i = 0; i < k; ++i) writer.write((remainder >> i) & 1);
        } else if (codec_name.find("exp_golomb_") == 0) {
            unsigned order = std::stoi(codec_name.substr(11));
            uint64_t v = value + (1ULL << order);
            int len = 0;
            uint64_t tmp = v;
            while (tmp > 0) { ++len; tmp >>= 1; }
            for (int i = 0; i < len - 1; ++i) writer.write(false);
            for (int i = len - 1; i >= 0; --i) writer.write((v >> i) & 1);
        } else if (codec_name.find("fixed_") == 0) {
            unsigned width = std::stoi(codec_name.substr(6));
            writer.write_bits(value, width);
        } else {
            throw std::invalid_argument("Unsupported codec for compress: " + codec_name);
        }
    }
    writer.align();

    return py::bytes(reinterpret_cast<const char*>(buffer.data()),
                     writer.bytes_written());
}

std::vector<uint64_t> decompress(py::bytes data, const std::string& codec_name) {
    char* ptr;
    Py_ssize_t size;
    if (PyBytes_AsStringAndSize(data.ptr(), &ptr, &size) < 0) {
        throw py::error_already_set();
    }

    pfc::BitReader reader(reinterpret_cast<const uint8_t*>(ptr), size);

    // Read count (VByte-style)
    size_t count = 0;
    int shift = 0;
    uint8_t byte;
    do {
        byte = static_cast<uint8_t>(reader.read_bits(8));
        count |= static_cast<size_t>(byte & 0x7F) << shift;
        shift += 7;
    } while (byte & 0x80);

    // Decode all values
    std::vector<uint64_t> result;
    result.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        uint64_t value;
        if (codec_name == "elias_gamma") {
            value = pfc::codecs::EliasGamma::decode<uint64_t>(reader);
        } else if (codec_name == "elias_delta") {
            value = pfc::codecs::EliasDelta::decode<uint64_t>(reader);
        } else if (codec_name == "fibonacci") {
            value = pfc::codecs::Fibonacci::decode<uint64_t>(reader);
        } else if (codec_name == "unary") {
            value = pfc::codecs::Unary::decode<uint64_t>(reader);
        } else if (codec_name.find("rice_") == 0) {
            unsigned k = std::stoi(codec_name.substr(5));
            uint64_t quotient = 0;
            while (!reader.read()) ++quotient;
            uint64_t remainder = 0;
            for (unsigned j = 0; j < k; ++j) remainder |= static_cast<uint64_t>(reader.read()) << j;
            value = (quotient << k) | remainder;
        } else if (codec_name.find("exp_golomb_") == 0) {
            unsigned order = std::stoi(codec_name.substr(11));
            int zeros = 0;
            while (!reader.read()) ++zeros;
            uint64_t v = 1;
            for (int j = 0; j < zeros; ++j) v = (v << 1) | reader.read();
            value = v - (1ULL << order);
        } else if (codec_name.find("fixed_") == 0) {
            unsigned width = std::stoi(codec_name.substr(6));
            value = reader.read_bits(width);
        } else {
            throw std::invalid_argument("Unsupported codec for decompress: " + codec_name);
        }
        result.push_back(value);
    }

    return result;
}

// Initialize codec registry with all standard codecs
void init_codec_registry() {
    auto& registry = CodecRegistry::instance();

    // Universal codes
    registry.register_codec("unary", make_codec<pfc::codecs::Unary>("unary"));
    registry.register_codec("elias_gamma", make_codec<pfc::codecs::EliasGamma>("elias_gamma"));
    registry.register_codec("elias_delta", make_codec<pfc::codecs::EliasDelta>("elias_delta"));
    registry.register_codec("fibonacci", make_codec<pfc::codecs::Fibonacci>("fibonacci"));

    // Pre-registered Rice codecs with common k values
    for (unsigned k = 0; k <= 8; ++k) {
        registry.register_codec("rice_" + std::to_string(k), make_rice_codec(k));
    }

    // Pre-registered ExpGolomb codecs with common orders
    for (unsigned order = 0; order <= 4; ++order) {
        registry.register_codec("exp_golomb_" + std::to_string(order), make_exp_golomb_codec(order));
    }

    // Pre-registered fixed-width codecs
    for (unsigned width : {8, 16, 32, 64}) {
        registry.register_codec("fixed_" + std::to_string(width), make_fixed_codec(width));
    }
}

// Register codec bindings
void register_codecs(py::module_& m) {
    // Initialize registry
    init_codec_registry();

    // Create codecs submodule
    auto codecs = m.def_submodule("codecs", "Prefix-free codec implementations");

    // PyCodec class
    py::class_<PyCodec>(codecs, "Codec",
        "A prefix-free codec for encoding/decoding integers")
        .def("encode", &PyCodec::encode, py::arg("value"), py::arg("writer"),
            "Encode an unsigned integer")
        .def("decode", &PyCodec::decode, py::arg("reader"),
            "Decode an unsigned integer")
        .def_property_readonly("name", &PyCodec::name,
            "Get the codec name");

    // SignedCodec class
    py::class_<SignedCodec>(codecs, "SignedCodec",
        "Wrapper for encoding signed integers using zigzag encoding")
        .def(py::init<PyCodec>(), py::arg("base_codec"),
            "Create a signed codec from a base unsigned codec")
        .def("encode", &SignedCodec::encode, py::arg("value"), py::arg("writer"),
            "Encode a signed integer")
        .def("decode", &SignedCodec::decode, py::arg("reader"),
            "Decode a signed integer");

    // Codec instances as module attributes
    codecs.attr("unary") = CodecRegistry::instance().get("unary");
    codecs.attr("elias_gamma") = CodecRegistry::instance().get("elias_gamma");
    codecs.attr("elias_delta") = CodecRegistry::instance().get("elias_delta");
    codecs.attr("fibonacci") = CodecRegistry::instance().get("fibonacci");

    // Factory functions for parameterized codecs
    codecs.def("rice", &make_rice_codec, py::arg("k") = 3,
        "Create a Rice codec with parameter k");

    codecs.def("exp_golomb", &make_exp_golomb_codec, py::arg("order") = 0,
        "Create an Exponential-Golomb codec with specified order");

    codecs.def("fixed", &make_fixed_codec, py::arg("width") = 32,
        "Create a fixed-width codec with specified bit width");

    // Signed variants
    codecs.attr("signed_gamma") = SignedCodec(CodecRegistry::instance().get("elias_gamma"));
    codecs.attr("signed_delta") = SignedCodec(CodecRegistry::instance().get("elias_delta"));

    // List available codecs
    codecs.def("list_codecs", []() {
        return CodecRegistry::instance().list_codecs();
    }, "List all registered codec names");

    codecs.def("get", [](const std::string& name) {
        return CodecRegistry::instance().get(name);
    }, py::arg("name"), "Get a codec by name");

    // High-level compress/decompress on main module
    m.def("compress", &compress, py::arg("data"), py::arg("codec") = "elias_gamma",
        R"doc(
        Compress a list of unsigned integers using the specified codec.

        Args:
            data: List of unsigned integers to compress
            codec: Codec name (default: 'elias_gamma')

        Returns:
            Compressed bytes

        Example:
            >>> compressed = pfc.compress([1, 2, 3, 5, 8, 13], codec='elias_gamma')
        )doc");

    m.def("decompress", &decompress, py::arg("data"), py::arg("codec") = "elias_gamma",
        R"doc(
        Decompress bytes to a list of unsigned integers.

        Args:
            data: Compressed bytes
            codec: Codec name (must match what was used to compress)

        Returns:
            List of unsigned integers

        Example:
            >>> data = pfc.decompress(compressed, codec='elias_gamma')
        )doc");
}

} // namespace pfc_python
