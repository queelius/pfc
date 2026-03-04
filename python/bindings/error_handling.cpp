// error_handling.cpp - Exception translation for PFC Python bindings

#include <pybind11/pybind11.h>
#include <pfc/error_handling.hpp>
#include <stdexcept>

namespace py = pybind11;

namespace pfc_python {

// Custom exception types for Python
struct PFCError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct BufferOverflowError : PFCError {
    using PFCError::PFCError;
};

struct CorruptedDataError : PFCError {
    using PFCError::PFCError;
};

struct InvalidArgumentError : PFCError {
    using PFCError::PFCError;
};

// Register exception types with pybind11
void register_exceptions(py::module_& m) {
    // Base PFC error
    static py::exception<PFCError> pfc_error(m, "PFCError");

    // Derived exceptions
    static py::exception<BufferOverflowError> buffer_overflow(
        m, "BufferOverflowError", pfc_error.ptr());

    static py::exception<CorruptedDataError> corrupted_data(
        m, "CorruptedDataError", pfc_error.ptr());

    static py::exception<InvalidArgumentError> invalid_argument(
        m, "InvalidArgumentError", pfc_error.ptr());

    // Register exception translators
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const BufferOverflowError& e) {
            PyErr_SetString(buffer_overflow.ptr(), e.what());
        } catch (const CorruptedDataError& e) {
            PyErr_SetString(corrupted_data.ptr(), e.what());
        } catch (const InvalidArgumentError& e) {
            PyErr_SetString(invalid_argument.ptr(), e.what());
        } catch (const PFCError& e) {
            PyErr_SetString(pfc_error.ptr(), e.what());
        }
    });
}

// Helper to throw Python exceptions from C++ code
void throw_buffer_overflow(const char* msg) {
    throw BufferOverflowError(msg);
}

void throw_corrupted_data(const char* msg) {
    throw CorruptedDataError(msg);
}

void throw_invalid_argument(const char* msg) {
    throw InvalidArgumentError(msg);
}

} // namespace pfc_python
