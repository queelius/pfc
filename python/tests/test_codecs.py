"""Tests for PFC codec functionality."""

import pytest


class TestEliasGamma:
    """Test Elias Gamma codec."""

    def test_encode_decode_roundtrip(self):
        """Test encode/decode roundtrip."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        test_values = [0, 1, 2, 5, 10, 100, 1000, 10000]

        for value in test_values:
            writer = BitWriter(1024)
            codecs.elias_gamma.encode(value, writer)
            writer.align()

            reader = BitReader(writer.to_bytes())
            decoded = codecs.elias_gamma.decode(reader)

            assert decoded == value, f"Failed for value {value}"

    def test_sequence(self):
        """Test encoding a sequence of values."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        values = [1, 2, 3, 5, 8, 13, 21, 34]

        writer = BitWriter(1024)
        for v in values:
            codecs.elias_gamma.encode(v, writer)
        writer.align()

        reader = BitReader(writer.to_bytes())
        for expected in values:
            assert codecs.elias_gamma.decode(reader) == expected


class TestEliasDelta:
    """Test Elias Delta codec."""

    def test_encode_decode_roundtrip(self):
        """Test encode/decode roundtrip."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        test_values = [0, 1, 2, 5, 10, 100, 1000, 10000, 100000]

        for value in test_values:
            writer = BitWriter(1024)
            codecs.elias_delta.encode(value, writer)
            writer.align()

            reader = BitReader(writer.to_bytes())
            decoded = codecs.elias_delta.decode(reader)

            assert decoded == value, f"Failed for value {value}"


class TestFibonacci:
    """Test Fibonacci codec."""

    def test_encode_decode_roundtrip(self):
        """Test encode/decode roundtrip."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        test_values = [0, 1, 2, 5, 10, 100, 1000]

        for value in test_values:
            writer = BitWriter(1024)
            codecs.fibonacci.encode(value, writer)
            writer.align()

            reader = BitReader(writer.to_bytes())
            decoded = codecs.fibonacci.decode(reader)

            assert decoded == value, f"Failed for value {value}"


class TestRice:
    """Test Rice codec."""

    def test_with_various_k(self):
        """Test Rice codec with various k values."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        for k in range(5):
            codec = codecs.rice(k)
            test_values = [0, 1, 2, 7, 15, 31, 100]

            for value in test_values:
                writer = BitWriter(1024)
                codec.encode(value, writer)
                writer.align()

                reader = BitReader(writer.to_bytes())
                decoded = codec.decode(reader)

                assert decoded == value, f"Failed for k={k}, value={value}"


class TestExpGolomb:
    """Test Exponential-Golomb codec."""

    def test_order_0(self):
        """Test ExpGolomb with order 0 (same as Elias Gamma)."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        codec = codecs.exp_golomb(0)
        test_values = [0, 1, 2, 5, 10, 100]

        for value in test_values:
            writer = BitWriter(1024)
            codec.encode(value, writer)
            writer.align()

            reader = BitReader(writer.to_bytes())
            decoded = codec.decode(reader)

            assert decoded == value, f"Failed for value {value}"

    def test_various_orders(self):
        """Test ExpGolomb with various orders."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        for order in range(4):
            codec = codecs.exp_golomb(order)
            test_values = [0, 1, 2, 7, 15, 31, 100]

            for value in test_values:
                writer = BitWriter(1024)
                codec.encode(value, writer)
                writer.align()

                reader = BitReader(writer.to_bytes())
                decoded = codec.decode(reader)

                assert decoded == value, f"Failed for order={order}, value={value}"


class TestFixed:
    """Test fixed-width codec."""

    def test_8bit(self):
        """Test 8-bit fixed codec."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        codec = codecs.fixed(8)

        for value in [0, 1, 127, 255]:
            writer = BitWriter(1024)
            codec.encode(value, writer)
            writer.align()

            reader = BitReader(writer.to_bytes())
            assert codec.decode(reader) == value

    def test_32bit(self):
        """Test 32-bit fixed codec."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        codec = codecs.fixed(32)

        for value in [0, 1, 0xDEADBEEF, 0xFFFFFFFF]:
            writer = BitWriter(1024)
            codec.encode(value, writer)
            writer.align()

            reader = BitReader(writer.to_bytes())
            assert codec.decode(reader) == value


class TestSignedCodecs:
    """Test signed integer codecs."""

    def test_signed_gamma(self):
        """Test signed gamma codec."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        test_values = [0, 1, -1, 5, -5, 100, -100, 1000, -1000]

        for value in test_values:
            writer = BitWriter(1024)
            codecs.signed_gamma.encode(value, writer)
            writer.align()

            reader = BitReader(writer.to_bytes())
            decoded = codecs.signed_gamma.decode(reader)

            assert decoded == value, f"Failed for value {value}"

    def test_signed_delta(self):
        """Test signed delta codec."""
        from pfc_python._pfc import BitWriter, BitReader, codecs

        test_values = [0, 1, -1, 5, -5, 100, -100]

        for value in test_values:
            writer = BitWriter(1024)
            codecs.signed_delta.encode(value, writer)
            writer.align()

            reader = BitReader(writer.to_bytes())
            decoded = codecs.signed_delta.decode(reader)

            assert decoded == value, f"Failed for value {value}"


class TestHighLevelAPI:
    """Test high-level compress/decompress API."""

    def test_compress_decompress_elias_gamma(self):
        """Test compress/decompress with Elias Gamma."""
        from pfc_python._pfc import compress, decompress

        data = [1, 2, 3, 5, 8, 13, 21, 34]

        compressed = compress(data, 'elias_gamma')
        decompressed = decompress(compressed, 'elias_gamma')

        assert list(decompressed) == data

    def test_compress_decompress_elias_delta(self):
        """Test compress/decompress with Elias Delta."""
        from pfc_python._pfc import compress, decompress

        data = [1, 10, 100, 1000, 10000]

        compressed = compress(data, 'elias_delta')
        decompressed = decompress(compressed, 'elias_delta')

        assert list(decompressed) == data

    def test_compress_decompress_fibonacci(self):
        """Test compress/decompress with Fibonacci."""
        from pfc_python._pfc import compress, decompress

        data = [0, 1, 2, 3, 5, 8, 13]

        compressed = compress(data, 'fibonacci')
        decompressed = decompress(compressed, 'fibonacci')

        assert list(decompressed) == data

    def test_compression_ratio(self):
        """Test that compression actually reduces size."""
        from pfc_python._pfc import compress

        # Small values should compress well with Elias Gamma
        data = [1] * 1000
        compressed = compress(data, 'elias_gamma')

        # Each value=1 takes 1 bit in Elias Gamma, plus overhead
        # Original would be 8000 bytes (8 bytes per uint64)
        # Compressed should be much smaller
        assert len(compressed) < 4000  # At least 50% compression

    def test_empty_list(self):
        """Test compress/decompress with empty list."""
        from pfc_python._pfc import compress, decompress

        data = []
        compressed = compress(data, 'elias_gamma')
        decompressed = decompress(compressed, 'elias_gamma')

        assert list(decompressed) == data

    def test_large_sequence(self):
        """Test with a large sequence."""
        from pfc_python._pfc import compress, decompress
        import random

        # Generate 10000 small positive integers
        data = [random.randint(0, 1000) for _ in range(10000)]

        compressed = compress(data, 'elias_gamma')
        decompressed = decompress(compressed, 'elias_gamma')

        assert list(decompressed) == data


class TestCodecRegistry:
    """Test codec registry functionality."""

    def test_list_codecs(self):
        """Test listing available codecs."""
        from pfc_python._pfc import codecs

        available = codecs.list_codecs()

        # Check that expected codecs are present
        assert 'elias_gamma' in available
        assert 'elias_delta' in available
        assert 'fibonacci' in available
        assert 'unary' in available

    def test_get_codec_by_name(self):
        """Test getting codec by name."""
        from pfc_python._pfc import codecs

        gamma = codecs.get('elias_gamma')
        assert gamma.name == 'elias_gamma'

    def test_unknown_codec(self):
        """Test error on unknown codec."""
        from pfc_python._pfc import codecs

        with pytest.raises(Exception):
            codecs.get('nonexistent_codec')
