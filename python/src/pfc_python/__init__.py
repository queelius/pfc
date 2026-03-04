"""
PFC - Prefix-Free Codecs

Python bindings for the PFC C++20 library providing zero-copy,
prefix-free data representations with full algebraic type support.

Example usage:
    >>> import pfc
    >>> data = [1, 2, 3, 5, 8, 13, 21, 34]
    >>> compressed = pfc.compress(data, codec='elias_gamma')
    >>> decompressed = pfc.decompress(compressed, codec='elias_gamma')
    >>> assert data == decompressed
"""

from __future__ import annotations

__version__ = "1.0.0"
__author__ = "PFC Library Authors"

from pfc_python._pfc import (
    # Core
    BitWriter,
    BitReader,
    # Exceptions
    PFCError,
    BufferOverflowError,
    CorruptedDataError,
    # High-level API
    compress,
    decompress,
)

from pfc_python import codecs

__all__ = [
    # Version info
    "__version__",
    "__author__",
    # Core classes
    "BitWriter",
    "BitReader",
    # Exceptions
    "PFCError",
    "BufferOverflowError",
    "CorruptedDataError",
    # High-level API
    "compress",
    "decompress",
    # Submodules
    "codecs",
]
