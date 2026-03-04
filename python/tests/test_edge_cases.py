"""Tests for edge cases, error handling, and boundary conditions."""

import pytest
import numpy as np


class TestBitWriterEdgeCases:
    """Edge case tests for BitWriter."""

    def test_write_bits_count_exceeds_64_raises_error(self):
        """Test that write_bits raises error when count > 64."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        with pytest.raises(ValueError, match="count must be <= 64"):
            writer.write_bits(0, 65)

    def test_write_bits_count_zero(self):
        """Test write_bits with count=0 does nothing."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        writer.write_bits(0xFFFFFFFF, 0)
        assert writer.bytes_written() == 0

    def test_create_from_numpy_array(self):
        """Test creating BitWriter from numpy array."""
        from pfc_python._pfc import BitWriter
        buffer = np.zeros(100, dtype=np.uint8)
        writer = BitWriter(buffer)
        writer.write_bits(0xAB, 8)
        writer.align()
        assert buffer[0] == 0xAB

    def test_create_from_readonly_buffer_raises(self):
        """Test that read-only buffer raises error."""
        from pfc_python._pfc import BitWriter
        buffer = np.zeros(100, dtype=np.uint8)
        buffer.flags.writeable = False
        with pytest.raises((ValueError, TypeError)):
            BitWriter(buffer)

    def test_empty_writer(self):
        """Test empty BitWriter behavior."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        assert writer.bytes_written() == 0
        data = writer.to_bytes()
        assert len(data) == 0

    def test_align_at_byte_boundary(self):
        """Test align() when already at byte boundary."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        writer.write_bits(0xFF, 8)
        bytes_before = writer.bytes_written()
        writer.align()
        assert writer.bytes_written() == bytes_before

    def test_align_pads_with_zeros(self):
        """Test that align() pads partial bytes with zeros."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        writer.write(True)  # bit 0 = 1
        writer.write(True)  # bit 1 = 1
        writer.write(True)  # bit 2 = 1
        writer.align()
        data = writer.to_bytes()
        # Bits: 1, 1, 1, 0, 0, 0, 0, 0 (LSB first) = 0b00000111 = 7
        assert data[0] == 7

    def test_large_bit_sequence(self):
        """Test writing many bits."""
        from pfc_python._pfc import BitWriter, BitReader
        writer = BitWriter(4096)
        # Write 1000 alternating bits
        for i in range(1000):
            writer.write(i % 2 == 0)
        writer.align()

        reader = BitReader(writer.to_bytes())
        for i in range(1000):
            assert reader.read() == (i % 2 == 0)


class TestBitReaderEdgeCases:
    """Edge case tests for BitReader."""

    def test_read_bits_count_exceeds_64_raises_error(self):
        """Test that read_bits raises error when count > 64."""
        from pfc_python._pfc import BitReader
        reader = BitReader(bytes([0xFF] * 10))
        with pytest.raises(ValueError, match="count must be <= 64"):
            reader.read_bits(65)

    def test_read_bits_count_zero(self):
        """Test read_bits with count=0 returns 0."""
        from pfc_python._pfc import BitReader
        reader = BitReader(bytes([0xFF]))
        assert reader.read_bits(0) == 0
        # Should not have consumed any bits
        assert reader.has_more()

    def test_create_from_numpy_array(self):
        """Test creating BitReader from numpy array."""
        from pfc_python._pfc import BitReader
        buffer = np.array([0xAB, 0xCD], dtype=np.uint8)
        reader = BitReader(buffer)
        assert reader.read_bits(8) == 0xAB
        assert reader.read_bits(8) == 0xCD

    def test_empty_data(self):
        """Test BitReader with empty data."""
        from pfc_python._pfc import BitReader
        reader = BitReader(bytes())
        assert not reader.has_more()

    def test_align_skips_remaining_bits(self):
        """Test that align() skips to next byte boundary."""
        from pfc_python._pfc import BitReader
        reader = BitReader(bytes([0xFF, 0xAB]))
        reader.read()  # Read 1 bit
        reader.align()  # Skip remaining 7 bits
        assert reader.read_bits(8) == 0xAB

    def test_read_exactly_all_bits(self):
        """Test reading exactly all available bits."""
        from pfc_python._pfc import BitReader
        reader = BitReader(bytes([0xAB]))
        for i in range(8):
            assert reader.has_more()
            reader.read()
        assert not reader.has_more()


class TestCodecBoundaryValues:
    """Test codec boundary values and edge cases."""

    def test_elias_gamma_value_zero(self):
        """Test Elias Gamma with value 0."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        writer = BitWriter(1024)
        codecs.elias_gamma.encode(0, writer)
        writer.align()
        reader = BitReader(writer.to_bytes())
        assert codecs.elias_gamma.decode(reader) == 0

    def test_elias_gamma_large_value(self):
        """Test Elias Gamma with large values."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        large_values = [2**20, 2**30, 2**40]
        for value in large_values:
            writer = BitWriter(1024)
            codecs.elias_gamma.encode(value, writer)
            writer.align()
            reader = BitReader(writer.to_bytes())
            assert codecs.elias_gamma.decode(reader) == value

    def test_elias_delta_large_value(self):
        """Test Elias Delta with large values."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        large_values = [2**20, 2**30, 2**40, 2**50]
        for value in large_values:
            writer = BitWriter(1024)
            codecs.elias_delta.encode(value, writer)
            writer.align()
            reader = BitReader(writer.to_bytes())
            assert codecs.elias_delta.decode(reader) == value

    def test_fibonacci_consecutive_values(self):
        """Test Fibonacci codec with Fibonacci sequence values."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        fib_values = [1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610]
        for value in fib_values:
            writer = BitWriter(1024)
            codecs.fibonacci.encode(value, writer)
            writer.align()
            reader = BitReader(writer.to_bytes())
            assert codecs.fibonacci.decode(reader) == value

    def test_rice_k0_large_quotient(self):
        """Test Rice with k=0 and large quotient."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        codec = codecs.rice(0)
        # k=0 means all bits are unary - very expensive for large values
        value = 100
        writer = BitWriter(1024)
        codec.encode(value, writer)
        writer.align()
        reader = BitReader(writer.to_bytes())
        assert codec.decode(reader) == value

    def test_rice_various_k_values(self):
        """Test Rice codec with various k values and edge cases."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        for k in range(9):
            codec = codecs.rice(k)
            # Test value = 2^k - 1 (all remainder bits set)
            value = (1 << k) - 1 if k > 0 else 0
            writer = BitWriter(1024)
            codec.encode(value, writer)
            writer.align()
            reader = BitReader(writer.to_bytes())
            assert codec.decode(reader) == value

    def test_fixed_8bit_truncation(self):
        """Test fixed 8-bit codec truncates larger values."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        codec = codecs.fixed(8)
        # Writing value > max should truncate (only lower 8 bits)
        writer = BitWriter(1024)
        codec.encode(0x1FF, writer)  # 511 = 0x1FF
        writer.align()
        reader = BitReader(writer.to_bytes())
        decoded = codec.decode(reader)
        assert decoded == 0xFF  # Should be truncated to 255

    def test_fixed_various_widths(self):
        """Test fixed-width codec with various widths."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        test_cases = [
            (8, 0xFF),
            (16, 0xFFFF),
            (32, 0xFFFFFFFF),
            (64, 0xFFFFFFFFFFFFFFFF),
        ]
        for width, max_val in test_cases:
            codec = codecs.fixed(width)
            writer = BitWriter(1024)
            codec.encode(max_val, writer)
            writer.align()
            reader = BitReader(writer.to_bytes())
            assert codec.decode(reader) == max_val

    def test_unary_values(self):
        """Test unary codec with various values."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        for value in [0, 1, 2, 5, 10, 50]:
            writer = BitWriter(1024)
            codecs.unary.encode(value, writer)
            writer.align()
            reader = BitReader(writer.to_bytes())
            assert codecs.unary.decode(reader) == value


class TestSignedCodecsBoundary:
    """Test signed codec boundary values."""

    def test_signed_gamma_large_values(self):
        """Test signed gamma with large positive and negative values."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        test_values = [
            -(2**20),
            2**20,
            -(2**30),
            2**30,
        ]
        for value in test_values:
            writer = BitWriter(4096)
            codecs.signed_gamma.encode(value, writer)
            writer.align()
            reader = BitReader(writer.to_bytes())
            assert codecs.signed_gamma.decode(reader) == value

    def test_signed_delta_alternating_signs(self):
        """Test signed delta with alternating signs."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        values = [1, -1, 2, -2, 3, -3, 100, -100]
        writer = BitWriter(1024)
        for v in values:
            codecs.signed_delta.encode(v, writer)
        writer.align()
        reader = BitReader(writer.to_bytes())
        for expected in values:
            assert codecs.signed_delta.decode(reader) == expected

    def test_signed_gamma_zero(self):
        """Test signed gamma with zero."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        writer = BitWriter(1024)
        codecs.signed_gamma.encode(0, writer)
        writer.align()
        reader = BitReader(writer.to_bytes())
        assert codecs.signed_gamma.decode(reader) == 0

    def test_signed_zigzag_pattern(self):
        """Test that zigzag encoding produces expected pattern."""
        from pfc_python._pfc import BitWriter, BitReader, codecs
        # Zigzag: 0->0, -1->1, 1->2, -2->3, 2->4, ...
        # So small absolute values should compress well
        values = list(range(-50, 51))
        for value in values:
            writer = BitWriter(1024)
            codecs.signed_gamma.encode(value, writer)
            writer.align()
            reader = BitReader(writer.to_bytes())
            assert codecs.signed_gamma.decode(reader) == value


class TestHighLevelAPIEdgeCases:
    """Test high-level compress/decompress edge cases."""

    def test_compress_single_element(self):
        """Test compress with single element."""
        from pfc_python._pfc import compress, decompress
        data = [42]
        compressed = compress(data, 'elias_gamma')
        decompressed = decompress(compressed, 'elias_gamma')
        assert list(decompressed) == data

    def test_compress_with_rice_codec(self):
        """Test compress/decompress with Rice codec."""
        from pfc_python._pfc import compress, decompress
        data = [0, 1, 2, 3, 4, 5, 6, 7]
        compressed = compress(data, 'rice_3')
        decompressed = decompress(compressed, 'rice_3')
        assert list(decompressed) == data

    def test_compress_with_exp_golomb_codec(self):
        """Test compress/decompress with ExpGolomb codec."""
        from pfc_python._pfc import compress, decompress
        data = [0, 1, 2, 10, 100, 1000]
        compressed = compress(data, 'exp_golomb_2')
        decompressed = decompress(compressed, 'exp_golomb_2')
        assert list(decompressed) == data

    def test_compress_with_fixed_codec(self):
        """Test compress/decompress with fixed-width codec."""
        from pfc_python._pfc import compress, decompress
        data = [0, 127, 255]
        compressed = compress(data, 'fixed_8')
        decompressed = decompress(compressed, 'fixed_8')
        assert list(decompressed) == data

    def test_compress_invalid_codec_raises(self):
        """Test compress with invalid codec name raises error."""
        from pfc_python._pfc import compress
        with pytest.raises(Exception):
            compress([1, 2, 3], 'nonexistent_codec')

    def test_decompress_invalid_codec_raises(self):
        """Test decompress with invalid codec name raises error."""
        from pfc_python._pfc import compress, decompress
        compressed = compress([1, 2, 3], 'elias_gamma')
        with pytest.raises(Exception):
            decompress(compressed, 'nonexistent_codec')

    def test_compress_all_zeros(self):
        """Test compress with all zero values."""
        from pfc_python._pfc import compress, decompress
        data = [0] * 100
        compressed = compress(data, 'elias_gamma')
        decompressed = decompress(compressed, 'elias_gamma')
        assert list(decompressed) == data

    def test_compress_ascending_sequence(self):
        """Test compress with ascending sequence."""
        from pfc_python._pfc import compress, decompress
        data = list(range(100))
        compressed = compress(data, 'elias_delta')
        decompressed = decompress(compressed, 'elias_delta')
        assert list(decompressed) == data

    def test_compress_with_various_rice_k(self):
        """Test compress with various Rice k values."""
        from pfc_python._pfc import compress, decompress
        data = [1, 2, 3, 5, 8, 13, 21, 34]
        for k in range(9):
            codec = f'rice_{k}'
            compressed = compress(data, codec)
            decompressed = decompress(compressed, codec)
            assert list(decompressed) == data, f"Failed for {codec}"


class TestCodecRegistryComprehensive:
    """Comprehensive codec registry tests."""

    def test_all_universal_codecs_registered(self):
        """Test all universal codecs are registered."""
        from pfc_python._pfc import codecs
        available = codecs.list_codecs()
        universal = ['unary', 'elias_gamma', 'elias_delta', 'fibonacci']
        for codec_name in universal:
            assert codec_name in available

    def test_rice_codecs_registered(self):
        """Test Rice codecs are registered for k=0..8."""
        from pfc_python._pfc import codecs
        available = codecs.list_codecs()
        for k in range(9):
            assert f'rice_{k}' in available

    def test_exp_golomb_codecs_registered(self):
        """Test ExpGolomb codecs are registered for order=0..4."""
        from pfc_python._pfc import codecs
        available = codecs.list_codecs()
        for order in range(5):
            assert f'exp_golomb_{order}' in available

    def test_fixed_codecs_registered(self):
        """Test fixed-width codecs are registered."""
        from pfc_python._pfc import codecs
        available = codecs.list_codecs()
        for width in [8, 16, 32, 64]:
            assert f'fixed_{width}' in available

    def test_get_parameterized_codec(self):
        """Test getting parameterized codecs by name."""
        from pfc_python._pfc import codecs
        rice3 = codecs.get('rice_3')
        assert rice3.name == 'rice_3'

        exp2 = codecs.get('exp_golomb_2')
        assert exp2.name == 'exp_golomb_2'

        fixed32 = codecs.get('fixed_32')
        assert fixed32.name == 'fixed_32'

    def test_codec_count(self):
        """Test that expected number of codecs are registered."""
        from pfc_python._pfc import codecs
        available = codecs.list_codecs()
        # 4 universal + 9 rice + 5 exp_golomb + 4 fixed = 22 minimum
        assert len(available) >= 22
