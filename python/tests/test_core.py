"""Tests for PFC core functionality (BitWriter/BitReader)."""

import pytest
import numpy as np


class TestBitWriter:
    """Test BitWriter class."""

    def test_create_with_capacity(self):
        """Test creating BitWriter with capacity."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        assert writer.bytes_written() == 0

    def test_write_single_bit(self):
        """Test writing single bits."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        writer.write(True)
        writer.write(False)
        writer.write(True)
        writer.align()
        data = writer.to_bytes()
        # Bits: 1, 0, 1 (LSB first) = 0b101 = 5
        assert data[0] == 5

    def test_write_bits(self):
        """Test writing multiple bits at once."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        writer.write_bits(0b10110101, 8)
        writer.align()
        data = writer.to_bytes()
        assert data[0] == 0b10110101

    def test_bytes_written(self):
        """Test bytes_written tracking."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        assert writer.bytes_written() == 0
        writer.write_bits(0xFF, 8)
        assert writer.bytes_written() == 1
        writer.write_bits(0xFF, 8)
        assert writer.bytes_written() == 2

    def test_to_numpy(self):
        """Test conversion to numpy array."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        writer.write_bits(0xDEAD, 16)
        writer.align()
        arr = writer.to_numpy()
        assert isinstance(arr, np.ndarray)
        assert arr.dtype == np.uint8
        assert len(arr) == 2


class TestBitReader:
    """Test BitReader class."""

    def test_create_from_bytes(self):
        """Test creating BitReader from bytes."""
        from pfc_python._pfc import BitReader
        data = bytes([0b10110101])
        reader = BitReader(data)
        assert reader.has_more()

    def test_read_single_bit(self):
        """Test reading single bits."""
        from pfc_python._pfc import BitReader
        data = bytes([0b00000101])  # bits 0, 2 are 1 (LSB first)
        reader = BitReader(data)
        assert reader.read() == True   # bit 0
        assert reader.read() == False  # bit 1
        assert reader.read() == True   # bit 2

    def test_read_bits(self):
        """Test reading multiple bits at once."""
        from pfc_python._pfc import BitReader
        data = bytes([0b10110101, 0xFF])
        reader = BitReader(data)
        assert reader.read_bits(8) == 0b10110101
        assert reader.read_bits(8) == 0xFF

    def test_roundtrip(self):
        """Test write/read roundtrip."""
        from pfc_python._pfc import BitWriter, BitReader
        writer = BitWriter(1024)

        # Write some values
        writer.write(True)
        writer.write(False)
        writer.write_bits(0xABCD, 16)
        writer.write(True)
        writer.align()

        data = writer.to_bytes()
        reader = BitReader(data)

        # Read them back
        assert reader.read() == True
        assert reader.read() == False
        assert reader.read_bits(16) == 0xABCD
        assert reader.read() == True

    def test_has_more(self):
        """Test has_more() method."""
        from pfc_python._pfc import BitReader
        data = bytes([0xFF])
        reader = BitReader(data)
        assert reader.has_more()
        for _ in range(8):
            reader.read()
        assert not reader.has_more()


class TestRoundtrip:
    """Comprehensive roundtrip tests."""

    def test_many_bits(self):
        """Test roundtrip with many bits."""
        from pfc_python._pfc import BitWriter, BitReader
        import random

        writer = BitWriter(4096)
        bits = [random.choice([True, False]) for _ in range(1000)]

        for bit in bits:
            writer.write(bit)
        writer.align()

        reader = BitReader(writer.to_bytes())
        for expected in bits:
            assert reader.read() == expected

    def test_mixed_operations(self):
        """Test mixed single and multi-bit operations."""
        from pfc_python._pfc import BitWriter, BitReader

        writer = BitWriter(1024)

        # Mix of operations
        writer.write(True)
        writer.write_bits(42, 7)
        writer.write(False)
        writer.write_bits(0xDEADBEEF, 32)
        writer.align()

        reader = BitReader(writer.to_bytes())

        assert reader.read() == True
        assert reader.read_bits(7) == 42
        assert reader.read() == False
        assert reader.read_bits(32) == 0xDEADBEEF
