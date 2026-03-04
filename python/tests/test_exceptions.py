"""Tests for PFC exception types and error handling."""

import pytest


class TestExceptionTypesExist:
    """Test that custom exception types exist."""

    def test_pfc_error_exists(self):
        """Test PFCError exception exists."""
        from pfc_python._pfc import PFCError
        assert PFCError is not None

    def test_buffer_overflow_error_exists(self):
        """Test BufferOverflowError exception exists."""
        from pfc_python._pfc import BufferOverflowError
        assert BufferOverflowError is not None

    def test_corrupted_data_error_exists(self):
        """Test CorruptedDataError exception exists."""
        from pfc_python._pfc import CorruptedDataError
        assert CorruptedDataError is not None


class TestExceptionHierarchy:
    """Test exception hierarchy relationships."""

    def test_buffer_overflow_inherits_from_pfc_error(self):
        """Test BufferOverflowError inherits from PFCError."""
        from pfc_python._pfc import BufferOverflowError, PFCError
        assert issubclass(BufferOverflowError, PFCError)

    def test_corrupted_data_inherits_from_pfc_error(self):
        """Test CorruptedDataError inherits from PFCError."""
        from pfc_python._pfc import CorruptedDataError, PFCError
        assert issubclass(CorruptedDataError, PFCError)

    def test_pfc_error_inherits_from_exception(self):
        """Test PFCError inherits from Exception."""
        from pfc_python._pfc import PFCError
        assert issubclass(PFCError, Exception)


class TestExceptionCanBeRaised:
    """Test that exceptions can be raised and caught."""

    def test_pfc_error_can_be_raised(self):
        """Test PFCError can be raised."""
        from pfc_python._pfc import PFCError
        with pytest.raises(PFCError):
            raise PFCError("test error")

    def test_buffer_overflow_can_be_raised(self):
        """Test BufferOverflowError can be raised."""
        from pfc_python._pfc import BufferOverflowError
        with pytest.raises(BufferOverflowError):
            raise BufferOverflowError("buffer overflow")

    def test_corrupted_data_can_be_raised(self):
        """Test CorruptedDataError can be raised."""
        from pfc_python._pfc import CorruptedDataError
        with pytest.raises(CorruptedDataError):
            raise CorruptedDataError("corrupted data")


class TestExceptionCatching:
    """Test exception catching behavior."""

    def test_catching_pfc_error_catches_buffer_overflow(self):
        """Test catching PFCError catches BufferOverflowError."""
        from pfc_python._pfc import PFCError, BufferOverflowError
        with pytest.raises(PFCError):
            raise BufferOverflowError("buffer overflow")

    def test_catching_pfc_error_catches_corrupted_data(self):
        """Test catching PFCError catches CorruptedDataError."""
        from pfc_python._pfc import PFCError, CorruptedDataError
        with pytest.raises(PFCError):
            raise CorruptedDataError("corrupted data")

    def test_catching_exception_catches_pfc_error(self):
        """Test catching Exception catches PFCError."""
        from pfc_python._pfc import PFCError
        with pytest.raises(Exception):
            raise PFCError("test error")


class TestExceptionMessages:
    """Test exception message handling."""

    def test_pfc_error_preserves_message(self):
        """Test PFCError preserves error message."""
        from pfc_python._pfc import PFCError
        msg = "specific error message"
        try:
            raise PFCError(msg)
        except PFCError as e:
            assert msg in str(e)

    def test_buffer_overflow_preserves_message(self):
        """Test BufferOverflowError preserves error message."""
        from pfc_python._pfc import BufferOverflowError
        msg = "buffer is full"
        try:
            raise BufferOverflowError(msg)
        except BufferOverflowError as e:
            assert msg in str(e)


class TestErrorConditions:
    """Test that appropriate errors are raised in error conditions."""

    def test_invalid_bit_count_raises_error(self):
        """Test that write_bits with count > 64 raises ValueError."""
        from pfc_python._pfc import BitWriter
        writer = BitWriter(1024)
        with pytest.raises(ValueError):
            writer.write_bits(0, 65)

    def test_read_bits_invalid_count_raises_error(self):
        """Test that read_bits with count > 64 raises ValueError."""
        from pfc_python._pfc import BitReader
        reader = BitReader(bytes([0xFF] * 10))
        with pytest.raises(ValueError):
            reader.read_bits(65)

    def test_invalid_codec_name_raises_error(self):
        """Test that invalid codec name raises error."""
        from pfc_python._pfc import codecs
        with pytest.raises(Exception):
            codecs.get('invalid_codec_name')

    def test_compress_invalid_codec_raises_error(self):
        """Test that compress with invalid codec raises error."""
        from pfc_python._pfc import compress
        with pytest.raises(Exception):
            compress([1, 2, 3], 'nonexistent')

    def test_decompress_invalid_codec_raises_error(self):
        """Test that decompress with invalid codec raises error."""
        from pfc_python._pfc import compress, decompress
        # First create valid compressed data
        compressed = compress([1, 2, 3], 'elias_gamma')
        # Then try to decompress with invalid codec
        with pytest.raises(Exception):
            decompress(compressed, 'nonexistent')
