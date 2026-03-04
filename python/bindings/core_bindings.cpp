// core_bindings.cpp - BitWriter/BitReader bindings for PFC Python

#include "pfc_python.hpp"

namespace pfc_python {

// Register core bindings
void register_core(py::module_& m) {
    py::class_<PyBitWriter>(m, "BitWriter",
        R"doc(
        Bit-level writer for encoding data.

        Example:
            >>> writer = pfc.BitWriter(1024)  # 1KB buffer
            >>> writer.write(True)
            >>> writer.write_bits(0b10101, 5)
            >>> writer.align()
            >>> data = writer.to_bytes()
        )doc")
        .def(py::init<size_t>(), py::arg("capacity") = 1024,
            "Create a BitWriter with specified buffer capacity")
        .def(py::init<py::buffer>(), py::arg("buffer"),
            "Create a BitWriter using an existing writable buffer")
        .def("write", &PyBitWriter::write, py::arg("bit"),
            "Write a single bit (True=1, False=0)")
        .def("write_bits", &PyBitWriter::write_bits,
            py::arg("bits"), py::arg("count"),
            "Write multiple bits (LSB first)")
        .def("align", &PyBitWriter::align,
            "Align to byte boundary, padding with zeros")
        .def("bytes_written", &PyBitWriter::bytes_written,
            "Get number of bytes written (including partial byte)")
        .def("to_bytes", &PyBitWriter::to_bytes,
            "Get written data as bytes object")
        .def("to_numpy", &PyBitWriter::to_numpy,
            "Get written data as numpy array");

    py::class_<PyBitReader>(m, "BitReader",
        R"doc(
        Bit-level reader for decoding data.

        Example:
            >>> reader = pfc.BitReader(data)
            >>> bit = reader.read()
            >>> value = reader.read_bits(8)
            >>> reader.align()
        )doc")
        .def(py::init<py::bytes>(), py::arg("data"),
            "Create a BitReader from bytes")
        .def(py::init<py::buffer>(), py::arg("buffer"),
            "Create a BitReader from buffer (numpy array, etc.)")
        .def("read", &PyBitReader::read,
            "Read a single bit (returns bool)")
        .def("read_bits", &PyBitReader::read_bits, py::arg("count"),
            "Read multiple bits (LSB first, returns int)")
        .def("has_more", &PyBitReader::has_more,
            "Check if more bits are available")
        .def("align", &PyBitReader::align,
            "Skip to next byte boundary");
}

} // namespace pfc_python
