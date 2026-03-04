// pfc_python.hpp - Shared declarations for PFC Python bindings

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pfc/core.hpp>
#include <vector>
#include <memory>

namespace py = pybind11;

namespace pfc_python {

// Python-friendly wrapper for BitWriter
class PyBitWriter {
public:
    explicit PyBitWriter(size_t initial_capacity = 1024)
        : buffer_(initial_capacity) {
        writer_ = std::make_unique<pfc::BitWriter>(buffer_.data());
    }

    explicit PyBitWriter(py::buffer buffer) {
        py::buffer_info info = buffer.request();
        if (info.format != py::format_descriptor<uint8_t>::format() &&
            info.format != "B" && info.format != "b") {
            throw std::invalid_argument("Buffer must be uint8 (bytes)");
        }
        if (info.readonly) {
            throw std::invalid_argument("Buffer must be writable");
        }
        external_buffer_ = static_cast<uint8_t*>(info.ptr);
        external_size_ = static_cast<size_t>(info.size);
        writer_ = std::make_unique<pfc::BitWriter>(external_buffer_);
    }

    void write(bool bit) {
        writer_->write(bit);
    }

    void write_bits(uint64_t bits, size_t count) {
        if (count > 64) {
            throw std::invalid_argument("count must be <= 64");
        }
        writer_->write_bits(bits, count);
    }

    void align() {
        writer_->align();
    }

    size_t bytes_written() const {
        return writer_->bytes_written();
    }

    py::bytes to_bytes() {
        align();
        if (external_buffer_) {
            return py::bytes(reinterpret_cast<const char*>(external_buffer_),
                           bytes_written());
        } else {
            return py::bytes(reinterpret_cast<const char*>(buffer_.data()),
                           bytes_written());
        }
    }

    py::array_t<uint8_t> to_numpy() {
        align();
        size_t size = bytes_written();
        if (external_buffer_) {
            return py::array_t<uint8_t>(size, external_buffer_, py::none());
        } else {
            return py::array_t<uint8_t>(size, buffer_.data(), py::none());
        }
    }

    uint8_t* data() {
        return external_buffer_ ? external_buffer_ : buffer_.data();
    }

    pfc::BitWriter& get_writer() {
        return *writer_;
    }

private:
    std::vector<uint8_t> buffer_;
    uint8_t* external_buffer_ = nullptr;
    size_t external_size_ = 0;
    std::unique_ptr<pfc::BitWriter> writer_;
};

// Python-friendly wrapper for BitReader
class PyBitReader {
public:
    explicit PyBitReader(py::bytes data) {
        char* ptr;
        Py_ssize_t size;
        if (PyBytes_AsStringAndSize(data.ptr(), &ptr, &size) < 0) {
            throw py::error_already_set();
        }
        buffer_.assign(reinterpret_cast<uint8_t*>(ptr),
                       reinterpret_cast<uint8_t*>(ptr) + size);
        reader_ = std::make_unique<pfc::BitReader>(buffer_.data(), buffer_.size());
    }

    explicit PyBitReader(py::buffer buffer) {
        py::buffer_info info = buffer.request();
        if (info.format != py::format_descriptor<uint8_t>::format() &&
            info.format != "B" && info.format != "b") {
            throw std::invalid_argument("Buffer must be uint8 (bytes)");
        }
        const uint8_t* ptr = static_cast<const uint8_t*>(info.ptr);
        size_t size = static_cast<size_t>(info.size);
        buffer_.assign(ptr, ptr + size);
        reader_ = std::make_unique<pfc::BitReader>(buffer_.data(), buffer_.size());
    }

    bool read() {
        return reader_->read();
    }

    uint64_t read_bits(size_t count) {
        if (count > 64) {
            throw std::invalid_argument("count must be <= 64");
        }
        return reader_->read_bits(count);
    }

    bool has_more() const {
        return reader_->peek();
    }

    void align() {
        reader_->align();
    }

    pfc::BitReader& get_reader() {
        return *reader_;
    }

private:
    std::vector<uint8_t> buffer_;
    std::unique_ptr<pfc::BitReader> reader_;
};

} // namespace pfc_python
