# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

PFC (Prefix-Free Codecs) is a header-only C++20 library implementing zero-copy, prefix-free data representations with full algebraic type support and STL integration. The library follows Alex Stepanov's generic programming principles.

**Key invariant**: In-memory representation equals wire format (no marshaling).

## Commands

### Building
```bash
mkdir -p build && cd build
cmake .. -DCMAKE_CXX_STANDARD=20
cmake --build . -j4
```

### Running Tests
```bash
# All tests via CTest
ctest

# Individual test executables
./pfc_tests                 # Core: codecs, packed values, bit I/O
./test_advanced             # Algebraic types, STL integration
./test_production           # Huffman, LZ77, error handling, allocators
./test_stream_arithmetic    # Stream I/O, arithmetic coding
./test_crc                  # CRC checksums
./test_new_codecs           # VByte, ExpGolomb, EliasOmega
./test_succinct             # SuccinctBitVector with rank/select

# Run specific test sections (Catch2 tags)
./pfc_tests "[codecs]"
./pfc_tests "[core]"
./test_advanced "[algebraic]"
./test_advanced "[stl]"

# List available tests
./pfc_tests --list-tests
./pfc_tests --list-tags
```

### Examples
```bash
./tutorial          # Basic usage
./advanced_demo     # Advanced features (may have infinite loops - use with caution)
./new_codecs_demo   # New codec demonstrations
```

## Architecture

### Core Concepts (core.hpp)

The library is built on three foundational concepts:

```cpp
template<typename T>
concept BitSink = /* can write bits: write(bool), write_bits(value, count) */;

template<typename T>
concept BitSource = /* can read bits: read(), read_bits(count) */;

template<typename C, typename T>
concept Codec = /* transforms T to/from bits via encode/decode static methods */;
```

### Codec Pattern

All codecs follow this structure:
```cpp
struct SomeCodec {
    static constexpr bool is_fixed_size() { return /* true/false */; }
    static constexpr size_t max_encoded_bits() { return /* max bits */; }

    template<BitSink S>
    static void encode(ValueType v, S& sink);

    template<BitSource S>
    static ValueType decode(S& source);
};
```

### Library Structure

| Header | Purpose |
|--------|---------|
| `core.hpp` | BitReader/BitWriter, concepts |
| `codecs.hpp` | Universal codes: Elias, Fibonacci, Rice, VByte, ExpGolomb |
| `numeric_codecs.hpp` | Floating point, rational, complex, fixed decimal |
| `packed.hpp` | Packed value types and containers |
| `algebraic.hpp` | Sum, Product, Maybe, Either, Result types |
| `stl_integration.hpp` | STL-compatible containers, iterators, algorithms |
| `succinct.hpp` | SuccinctBitVector with O(1) rank/select |
| `huffman.hpp` | Huffman coding |
| `lz77.hpp` | LZ77 compression |
| `arithmetic_coding.hpp` | Arithmetic coding (production), range coding (experimental) |
| `crc.hpp` | CRC8, CRC16, CRC32 checksums |
| `pfc.hpp` | Main header - includes everything |

### Type Composition

Packed types compose via templates:
```cpp
using OptionalInt = PackedOptional<PackedInt>;
using EitherIntString = PackedEither<PackedInt, PackedString>;
using IntList = PackedVector<PackedRational>;
```

## Test Organization

Tests use Catch2 with sections and tags:

- `test_unified.cpp` → `pfc_tests` - Core functionality, all codecs
- `test_advanced.cpp` → `test_advanced` - Algebraic types, STL integration
- `test_production.cpp` → `test_production` - Huffman, LZ77, error handling
- `test_stream_arithmetic.cpp` → `test_stream_arithmetic` - Stream I/O, arithmetic coding
- `test_crc.cpp` → `test_crc` - CRC checksums
- `test_new_codecs.cpp` → `test_new_codecs` - VByte, ExpGolomb, EliasOmega
- `test_succinct.cpp` → `test_succinct` - SuccinctBitVector (3167 assertions)

Current status: 86% pass rate (6/7 suites), 3477+ assertions passing.

## Known Issues

1. **Range coding** (arithmetic_coding.hpp): Works only for short sequences (2-3 bytes). Use arithmetic coding instead, which is fully functional.

2. **advanced_demo**: May have infinite loops in certain sections.

3. **Floating point precision**: Some edge cases with very small/large values.

## Design Principles

1. **Zero-copy**: Wire format = memory format
2. **Prefix-free**: Self-delimiting codes, no length headers needed
3. **Compile-time composition**: Templates and concepts, no virtual dispatch
4. **Regular types**: All types are copyable, assignable, comparable (Stepanov's requirements)
5. **Orthogonal design**: Codecs, containers, and algorithms are independent
