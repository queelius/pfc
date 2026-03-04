"""
PFC Codecs Module

Provides access to all prefix-free codecs: Elias Gamma/Delta/Omega,
Fibonacci, Rice, VByte, ExpGolomb, and signed variants.
"""

from __future__ import annotations

from pfc_python._pfc.codecs import (
    # Universal codes
    elias_gamma,
    elias_delta,
    # elias_omega,  # TODO: bind in C++ when available
    fibonacci,
    unary,
    # vbyte,  # TODO: bind in C++ when available
    # Parameterized codecs (factory functions)
    rice,
    exp_golomb,
    fixed,
    # Signed variants
    signed_gamma,
    signed_delta,
)

__all__ = [
    "elias_gamma",
    "elias_delta",
    "fibonacci",
    "unary",
    "rice",
    "exp_golomb",
    "fixed",
    "signed_gamma",
    "signed_delta",
]
