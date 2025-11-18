#pragma once
// arithmetic_coding.hpp - Arithmetic and range coding for PFC library
// Optimal entropy coding with adaptive probability models
// "Information is the resolution of uncertainty" - Claude Shannon

#include "core.hpp"
#include "error_handling.hpp"
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <cstdint>
#include <concepts>
#include <type_traits>

namespace pfc {

// ============================================================
//  Probability Models - The foundation of arithmetic coding
// ============================================================

// Concept for probability models
template<typename M>
concept ProbabilityModel = requires(M& model, std::size_t symbol) {
    { model.symbol_count() } -> std::convertible_to<std::size_t>;
    { model.cumulative_frequency(symbol) } -> std::convertible_to<uint32_t>;
    { model.frequency(symbol) } -> std::convertible_to<uint32_t>;
    { model.total_frequency() } -> std::convertible_to<uint32_t>;
    { model.find_symbol(uint32_t{}) } -> std::convertible_to<std::size_t>;
    { model.update(symbol) } -> std::same_as<void>;
};

// Static probability model with fixed frequencies
template<std::size_t Symbols>
class static_probability_model {
    std::array<uint32_t, Symbols> frequencies_;
    std::array<uint32_t, Symbols + 1> cumulative_;
    uint32_t total_;

public:
    static_probability_model() : frequencies_{}, cumulative_{}, total_(0) {
        reset_uniform();
    }

    explicit static_probability_model(const std::array<uint32_t, Symbols>& freqs)
        : frequencies_(freqs) {
        build_cumulative();
    }

    void reset_uniform() {
        std::fill(frequencies_.begin(), frequencies_.end(), 1);
        build_cumulative();
    }

    void set_frequencies(const std::array<uint32_t, Symbols>& freqs) {
        frequencies_ = freqs;
        build_cumulative();
    }

    [[nodiscard]] std::size_t symbol_count() const noexcept { return Symbols; }

    [[nodiscard]] uint32_t frequency(std::size_t symbol) const noexcept {
        return frequencies_[symbol];
    }

    [[nodiscard]] uint32_t cumulative_frequency(std::size_t symbol) const noexcept {
        return cumulative_[symbol];
    }

    [[nodiscard]] uint32_t total_frequency() const noexcept {
        return total_;
    }

    [[nodiscard]] std::size_t find_symbol(uint32_t value) const noexcept {
        // Binary search for efficiency
        auto it = std::upper_bound(cumulative_.begin(), cumulative_.end() - 1, value);
        return std::distance(cumulative_.begin(), it) - 1;
    }

    void update(std::size_t) {
        // Static model doesn't update
    }

private:
    void build_cumulative() {
        cumulative_[0] = 0;
        for (std::size_t i = 0; i < Symbols; ++i) {
            cumulative_[i + 1] = cumulative_[i] + frequencies_[i];
        }
        total_ = cumulative_[Symbols];
    }
};

// Adaptive probability model that learns from data
template<std::size_t Symbols>
class adaptive_probability_model {
    std::array<uint32_t, Symbols> frequencies_;
    std::array<uint32_t, Symbols + 1> cumulative_;
    uint32_t total_;
    static constexpr uint32_t max_frequency = 16383;  // Prevent overflow
    static constexpr uint32_t rescale_threshold = max_frequency * Symbols;

public:
    adaptive_probability_model() : frequencies_{}, cumulative_{}, total_(0) {
        reset();
    }

    void reset() {
        std::fill(frequencies_.begin(), frequencies_.end(), 1);
        build_cumulative();
    }

    [[nodiscard]] std::size_t symbol_count() const noexcept { return Symbols; }

    [[nodiscard]] uint32_t frequency(std::size_t symbol) const noexcept {
        return frequencies_[symbol];
    }

    [[nodiscard]] uint32_t cumulative_frequency(std::size_t symbol) const noexcept {
        return cumulative_[symbol];
    }

    [[nodiscard]] uint32_t total_frequency() const noexcept {
        return total_;
    }

    [[nodiscard]] std::size_t find_symbol(uint32_t value) const noexcept {
        // Linear search for small alphabets, binary search for large ones
        if constexpr (Symbols <= 16) {
            for (std::size_t i = 0; i < Symbols; ++i) {
                if (value < cumulative_[i + 1]) return i;
            }
            return Symbols - 1;
        } else {
            auto it = std::upper_bound(cumulative_.begin(), cumulative_.end() - 1, value);
            return std::distance(cumulative_.begin(), it) - 1;
        }
    }

    void update(std::size_t symbol) {
        frequencies_[symbol]++;

        // Update cumulative frequencies from symbol onward
        for (std::size_t i = symbol + 1; i <= Symbols; ++i) {
            cumulative_[i]++;
        }
        total_++;

        // Rescale if necessary to prevent overflow
        if (total_ >= rescale_threshold) {
            rescale();
        }
    }

private:
    void build_cumulative() {
        cumulative_[0] = 0;
        for (std::size_t i = 0; i < Symbols; ++i) {
            cumulative_[i + 1] = cumulative_[i] + frequencies_[i];
        }
        total_ = cumulative_[Symbols];
    }

    void rescale() {
        // Halve all frequencies (keeping minimum of 1)
        for (auto& freq : frequencies_) {
            freq = (freq + 1) / 2;
        }
        build_cumulative();
    }
};

// Context model for order-N modeling
template<std::size_t Symbols, std::size_t Order>
class context_model {
    static constexpr std::size_t context_count = 1u << (Order * 8);
    std::vector<adaptive_probability_model<Symbols>> models_;
    std::size_t current_context_ = 0;

public:
    context_model() : models_(context_count) {}

    void reset() {
        for (auto& model : models_) {
            model.reset();
        }
        current_context_ = 0;
    }

    void set_context(const uint8_t* context_bytes) {
        current_context_ = 0;
        for (std::size_t i = 0; i < Order; ++i) {
            current_context_ = (current_context_ << 8) | context_bytes[i];
        }
        current_context_ %= context_count;
    }

    [[nodiscard]] auto& current_model() { return models_[current_context_]; }
    [[nodiscard]] const auto& current_model() const { return models_[current_context_]; }
};

// ============================================================
//  Arithmetic Encoder - The heart of optimal compression
// ============================================================

class arithmetic_encoder {
    static constexpr uint32_t top_value = 0x7FFFFFFF;  // 31 bits
    static constexpr uint32_t first_quarter = top_value / 4 + 1;
    static constexpr uint32_t half = 2 * first_quarter;
    static constexpr uint32_t third_quarter = 3 * first_quarter;

    BitWriter* sink_;
    uint32_t low_ = 0;
    uint32_t high_ = top_value;
    uint32_t bits_to_follow_ = 0;

    void output_bit_plus_follow(bool bit) {
        sink_->write(bit);
        while (bits_to_follow_ > 0) {
            sink_->write(!bit);
            bits_to_follow_--;
        }
    }

public:
    explicit arithmetic_encoder(BitWriter& sink) : sink_(&sink) {}

    template<ProbabilityModel Model>
    void encode_symbol(std::size_t symbol, Model& model) {
        uint32_t range = high_ - low_ + 1;
        uint32_t total = model.total_frequency();

        // Ensure we have a valid total frequency
        if (total == 0) {
            return;  // Can't encode with empty model
        }

        uint32_t cum_freq = model.cumulative_frequency(symbol);
        uint32_t freq = model.frequency(symbol);

        // Update range based on symbol probability
        // Use uint64_t to prevent overflow in intermediate calculations
        uint64_t r = range;
        uint32_t new_high = low_ + static_cast<uint32_t>((r * (cum_freq + freq)) / total) - 1;
        uint32_t new_low = low_ + static_cast<uint32_t>((r * cum_freq) / total);

        // Ensure high_ > low_ (prevent range collapse)
        if (new_high <= new_low) {
            new_high = new_low + 1;
        }

        high_ = new_high;
        low_ = new_low;

        // Normalize and output bits
        while (true) {
            if (high_ < half) {
                output_bit_plus_follow(false);
                low_ *= 2;
                high_ = high_ * 2 + 1;
            } else if (low_ >= half) {
                output_bit_plus_follow(true);
                low_ = (low_ - half) * 2;
                high_ = (high_ - half) * 2 + 1;
            } else if (low_ >= first_quarter && high_ < third_quarter) {
                bits_to_follow_++;
                low_ = (low_ - first_quarter) * 2;
                high_ = (high_ - first_quarter) * 2 + 1;
            } else {
                break;
            }
        }

        model.update(symbol);
    }

    void finish() {
        // Output final bits
        bits_to_follow_++;
        if (low_ < first_quarter) {
            output_bit_plus_follow(false);
        } else {
            output_bit_plus_follow(true);
        }
    }
};

// ============================================================
//  Arithmetic Decoder - Reversing the entropy coding
// ============================================================

class arithmetic_decoder {
    static constexpr uint32_t top_value = 0x7FFFFFFF;
    static constexpr uint32_t first_quarter = top_value / 4 + 1;
    static constexpr uint32_t half = 2 * first_quarter;
    static constexpr uint32_t third_quarter = 3 * first_quarter;

    BitReader* source_;
    uint32_t low_ = 0;
    uint32_t high_ = top_value;
    uint32_t value_ = 0;

    bool read_bit() {
        return source_->read();
    }

public:
    explicit arithmetic_decoder(BitReader& source) : source_(&source) {
        // Initialize decoder with first 31 bits
        for (int i = 0; i < 31; ++i) {
            value_ = (value_ << 1) | (read_bit() ? 1 : 0);
        }
    }

    template<ProbabilityModel Model>
    std::size_t decode_symbol(Model& model) {
        uint32_t range = high_ - low_ + 1;
        uint32_t total = model.total_frequency();

        // Ensure we have a valid total frequency
        if (total == 0) {
            return 0;  // Can't decode with empty model
        }

        // Ensure range is non-zero to prevent division by zero
        if (range == 0) {
            return 0;  // Invalid state
        }

        // Use uint64_t to prevent overflow in intermediate calculations
        // The encoder computes: new_low = low_ + (range * cum_freq) / total
        // To invert: cum_freq = ((value_ - low_) * total) / range
        uint64_t v = value_ - low_;
        uint32_t scaled_value = static_cast<uint32_t>((v * total) / range);

        // Find symbol
        std::size_t symbol = model.find_symbol(scaled_value);

        // Update range
        uint32_t cum_freq = model.cumulative_frequency(symbol);
        uint32_t freq = model.frequency(symbol);

        uint64_t r = range;
        uint32_t new_high = low_ + static_cast<uint32_t>((r * (cum_freq + freq)) / total) - 1;
        uint32_t new_low = low_ + static_cast<uint32_t>((r * cum_freq) / total);

        // Ensure high_ > low_ (prevent range collapse)
        if (new_high <= new_low) {
            new_high = new_low + 1;
        }

        high_ = new_high;
        low_ = new_low;

        // Normalize and read new bits
        while (true) {
            if (high_ < half) {
                low_ *= 2;
                high_ = high_ * 2 + 1;
                value_ = (value_ * 2) | (read_bit() ? 1 : 0);
            } else if (low_ >= half) {
                low_ = (low_ - half) * 2;
                high_ = (high_ - half) * 2 + 1;
                value_ = ((value_ - half) * 2) | (read_bit() ? 1 : 0);
            } else if (low_ >= first_quarter && high_ < third_quarter) {
                low_ = (low_ - first_quarter) * 2;
                high_ = (high_ - first_quarter) * 2 + 1;
                value_ = ((value_ - first_quarter) * 2) | (read_bit() ? 1 : 0);
            } else {
                break;
            }
        }

        model.update(symbol);
        return symbol;
    }
};

// ============================================================
//  Range Coder - More efficient variant of arithmetic coding
// ============================================================

class range_encoder {
    static constexpr uint64_t top = 1ULL << 56;      // Use 56-bit range
    static constexpr uint64_t bottom = 1ULL << 48;   // Normalize when range < 2^48

    BitWriter* sink_;
    uint64_t low_ = 0;
    uint64_t range_ = top;

    void normalize() {
        while (range_ < bottom) {
            // Output top byte
            sink_->write_bits((low_ >> 56) & 0xFF, 8);
            low_ <<= 8;
            range_ <<= 8;
        }
    }

public:
    explicit range_encoder(BitWriter& sink) : sink_(&sink) {}

    template<ProbabilityModel Model>
    void encode_symbol(std::size_t symbol, Model& model) {
        uint32_t total = model.total_frequency();

        if (total == 0) {
            return;  // Can't encode with empty model
        }

        uint32_t cum_freq = model.cumulative_frequency(symbol);
        uint32_t freq = model.frequency(symbol);

        // Simple 64-bit arithmetic, no overflow possible
        low_ += (range_ * cum_freq) / total;
        range_ = (range_ * freq) / total;

        if (range_ == 0) range_ = 1;

        normalize();
        model.update(symbol);
    }

    void encode_bit(bool bit, uint32_t prob_zero) {
        uint32_t prob_scale = 0x10000;

        if (!bit) {
            range_ = (range_ * prob_zero) / prob_scale;
        } else {
            low_ += (range_ * prob_zero) / prob_scale;
            range_ = (range_ * (prob_scale - prob_zero)) / prob_scale;
        }

        if (range_ == 0) range_ = 1;
        normalize();
    }

    void finish() {
        // Output remaining bytes (7 bytes for 56-bit value)
        for (int i = 0; i < 7; ++i) {
            sink_->write_bits((low_ >> 56) & 0xFF, 8);
            low_ <<= 8;
        }
    }
};

class range_decoder {
    static constexpr uint64_t top = 1ULL << 56;      // Match encoder
    static constexpr uint64_t bottom = 1ULL << 48;   // Match encoder

    BitReader* source_;
    uint64_t low_ = 0;
    uint64_t code_ = 0;
    uint64_t range_ = top;

    void normalize() {
        while (range_ < bottom) {
            code_ = (code_ << 8) | source_->read_bits(8);
            low_ <<= 8;
            range_ <<= 8;
        }
    }

public:
    explicit range_decoder(BitReader& source) : source_(&source) {
        // Initialize with first 7 bytes (56 bits)
        for (int i = 0; i < 7; ++i) {
            code_ = (code_ << 8) | source_->read_bits(8);
        }
    }

    template<ProbabilityModel Model>
    std::size_t decode_symbol(Model& model) {
        uint32_t total = model.total_frequency();

        if (total == 0) {
            return 0;  // Can't decode with empty model
        }

        // Find symbol by computing scaled value
        // Use the same formula as encoder to avoid rounding errors
        uint64_t helper = ((code_ - low_) + 1) * total - 1;
        uint64_t scaled_value = helper / range_;

        // Find symbol
        std::size_t symbol = model.find_symbol(static_cast<uint32_t>(scaled_value));

        // Update range - must match encoder exactly
        uint32_t cum_freq = model.cumulative_frequency(symbol);
        uint32_t freq = model.frequency(symbol);

        low_ += (range_ * cum_freq) / total;
        range_ = (range_ * freq) / total;

        if (range_ == 0) range_ = 1;  // Prevent range collapse

        normalize();
        model.update(symbol);
        return symbol;
    }

    bool decode_bit(uint32_t prob_zero) {
        uint32_t prob_scale = 0x10000;

        uint64_t threshold = low_ + (range_ * prob_zero) / prob_scale;

        bool bit = (code_ >= threshold);

        if (!bit) {
            range_ = (range_ * prob_zero) / prob_scale;
        } else {
            low_ = threshold;
            range_ = (range_ * (prob_scale - prob_zero)) / prob_scale;
        }

        if (range_ == 0) range_ = 1;
        normalize();
        return bit;
    }
};

// ============================================================
//  High-Level API - Easy to use interfaces
// ============================================================

// Compress bytes using arithmetic coding with adaptive model
template<typename InputIt, typename OutputIt>
result<std::size_t> arithmetic_compress(InputIt first, InputIt last, OutputIt output) {
    try {
        std::vector<uint8_t> buffer;
        buffer.reserve(std::distance(first, last) + 1024);

        BitWriter writer(buffer.data());
        arithmetic_encoder encoder(writer);
        adaptive_probability_model<256> model;

        // Encode each byte
        for (auto it = first; it != last; ++it) {
            encoder.encode_symbol(static_cast<uint8_t>(*it), model);
        }

        encoder.finish();
        writer.align();

        // Copy to output
        std::copy(buffer.begin(), buffer.begin() + writer.bytes_written(), output);

        return writer.bytes_written();
    } catch (const std::exception&) {
        return make_error_code(pfc_error::compression_error);
    }
}

// Decompress bytes using arithmetic coding with adaptive model
template<typename InputIt, typename OutputIt>
result<std::size_t> arithmetic_decompress(InputIt first, InputIt last, OutputIt output, std::size_t count) {
    try {
        std::vector<uint8_t> input_buffer(first, last);
        BitReader reader(input_buffer.data(), input_buffer.size());
        arithmetic_decoder decoder(reader);
        adaptive_probability_model<256> model;

        // Decode bytes
        for (std::size_t i = 0; i < count; ++i) {
            auto symbol = decoder.decode_symbol(model);
            *output++ = static_cast<uint8_t>(symbol);
        }

        return count;
    } catch (const std::exception&) {
        return make_error_code(pfc_error::decompression_error);
    }
}

// Range coding variants
template<typename InputIt, typename OutputIt>
result<std::size_t> range_compress(InputIt first, InputIt last, OutputIt output) {
    try {
        std::vector<uint8_t> buffer;
        buffer.reserve(std::distance(first, last) + 1024);

        BitWriter writer(buffer.data());
        range_encoder encoder(writer);
        adaptive_probability_model<256> model;

        for (auto it = first; it != last; ++it) {
            encoder.encode_symbol(static_cast<uint8_t>(*it), model);
        }

        encoder.finish();
        writer.align();

        std::copy(buffer.begin(), buffer.begin() + writer.bytes_written(), output);

        return writer.bytes_written();
    } catch (const std::exception&) {
        return make_error_code(pfc_error::compression_error);
    }
}

template<typename InputIt, typename OutputIt>
result<std::size_t> range_decompress(InputIt first, InputIt last, OutputIt output, std::size_t count) {
    try {
        std::vector<uint8_t> input_buffer(first, last);
        BitReader reader(input_buffer.data(), input_buffer.size());
        range_decoder decoder(reader);
        adaptive_probability_model<256> model;

        for (std::size_t i = 0; i < count; ++i) {
            auto symbol = decoder.decode_symbol(model);
            *output++ = static_cast<uint8_t>(symbol);
        }

        return count;
    } catch (const std::exception&) {
        return make_error_code(pfc_error::decompression_error);
    }
}

// ============================================================
//  Codec Interface - Integration with PFC codec system
// ============================================================

// Arithmetic codec for single values (using static model)
template<typename T, std::size_t Symbols = 256>
struct ArithmeticCodec {
    using value_type = T;

    template<BitSink Sink>
    static void encode(const T& value, Sink& sink) {
        static_probability_model<Symbols> model;
        arithmetic_encoder encoder(sink);

        // For simple types, encode bytes
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
        for (std::size_t i = 0; i < sizeof(T); ++i) {
            encoder.encode_symbol(bytes[i], model);
        }

        encoder.finish();
    }

    template<BitSource Source>
    static T decode(Source& source) {
        static_probability_model<Symbols> model;
        arithmetic_decoder decoder(source);

        T result;
        uint8_t* bytes = reinterpret_cast<uint8_t*>(&result);
        for (std::size_t i = 0; i < sizeof(T); ++i) {
            bytes[i] = static_cast<uint8_t>(decoder.decode_symbol(model));
        }

        return result;
    }
};

// Range codec variant
template<typename T, std::size_t Symbols = 256>
struct RangeCodec {
    using value_type = T;

    template<BitSink Sink>
    static void encode(const T& value, Sink& sink) {
        adaptive_probability_model<Symbols> model;
        range_encoder encoder(sink);

        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
        for (std::size_t i = 0; i < sizeof(T); ++i) {
            encoder.encode_symbol(bytes[i], model);
        }

        encoder.finish();
    }

    template<BitSource Source>
    static T decode(Source& source) {
        adaptive_probability_model<Symbols> model;
        range_decoder decoder(source);

        T result;
        uint8_t* bytes = reinterpret_cast<uint8_t*>(&result);
        for (std::size_t i = 0; i < sizeof(T); ++i) {
            bytes[i] = static_cast<uint8_t>(decoder.decode_symbol(model));
        }

        return result;
    }
};

} // namespace pfc