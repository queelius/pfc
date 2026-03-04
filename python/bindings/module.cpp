// module.cpp - Main pybind11 module for PFC Python bindings

#include <pybind11/pybind11.h>

namespace py = pybind11;

// Forward declarations from other binding files
namespace pfc_python {
    void register_exceptions(py::module_& m);
    void register_core(py::module_& m);
    void register_codecs(py::module_& m);
}

PYBIND11_MODULE(_pfc, m) {
    m.doc() = R"doc(
        PFC - Prefix-Free Codecs

        Python bindings for the PFC C++20 library providing zero-copy,
        prefix-free data representations with full algebraic type support.

        Key features:
        - Zero-copy invariant: in-memory representation = wire representation
        - Prefix-free codes: self-delimiting, streaming-friendly
        - Rich codec library: Elias, Fibonacci, Rice, VByte, and more
        - STL integration: works seamlessly with standard algorithms

        Example:
            >>> import pfc
            >>> data = [1, 2, 3, 5, 8, 13, 21, 34]
            >>> compressed = pfc.compress(data, codec='elias_gamma')
            >>> decompressed = pfc.decompress(compressed, codec='elias_gamma')
            >>> assert data == list(decompressed)
    )doc";

    // Version info
    m.attr("__version__") = "1.0.0";

    // Register all bindings
    pfc_python::register_exceptions(m);
    pfc_python::register_core(m);
    pfc_python::register_codecs(m);
}
