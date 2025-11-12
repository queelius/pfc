# API Reference Overview

PFC provides a comprehensive set of headers organized by functionality. This page gives an overview of the library structure and what each module provides.

## Header Organization

```
include/pfc/
├── pfc.hpp                   # Main header (includes everything)
├── core.hpp                  # Fundamental abstractions
├── codecs.hpp                # Universal integer codes
├── numeric_codecs.hpp        # Numeric type codecs
├── packed.hpp                # Packed value types
├── algebraic.hpp             # Algebraic type system
├── stl_integration.hpp       # STL compatibility
├── algorithms.hpp            # Generic algorithms
├── coordinates.hpp           # Vectors, matrices, transforms
├── error_handling.hpp        # Result and error types
├── huffman.hpp               # Huffman coding
├── lz77.hpp                  # LZ77 compression
├── crc.hpp                   # CRC checksums
├── stream_io.hpp             # Stream-based I/O
├── arithmetic_coding.hpp     # Arithmetic coding
└── allocator_support.hpp     # Custom allocator support
```

## Quick Reference

### Including the Library

```cpp
#include <pfc/pfc.hpp>          // Everything
#include <pfc/core.hpp>         // Just core functionality
#include <pfc/codecs.hpp>       // Just codecs
```

### Namespace

All PFC symbols are in the `pfc` namespace:

```cpp
using namespace pfc;  // or
pfc::EliasGamma       // explicit qualification
```

## Core Module

**Header**: `core.hpp`

Fundamental building blocks:

- **BitWriter**: Write bits to a buffer
- **BitReader**: Read bits from a buffer
- **Concepts**: BitSink, BitSource, Codec, PackedValue
- **Utilities**: Bit manipulation, alignment

[Detailed Core API →](core.md)

## Codecs Module

**Header**: `codecs.hpp`

Universal integer codes and fixed-width encodings:

- **Unary**: Simple unary code
- **EliasGamma**: Good for small integers
- **EliasDelta**: Good for medium integers
- **EliasOmega**: Good for mixed distributions
- **Fibonacci**: Based on Fibonacci sequence
- **Rice/Golomb**: Parametric codes
- **FixedWidth<N>**: Fixed N-bit encoding
- **Signed<Codec>**: Zigzag encoding for signed values

[Detailed Codecs API →](codecs.md)

## Numeric Codecs Module

**Header**: `numeric_codecs.hpp`

Floating point and numeric types:

- **FloatingPoint<M, E>**: Configurable precision
- **Float16, Float32, Float64**: Standard formats
- **FixedDecimal**: Exact decimal representation
- **Rational**: Fraction encoding
- **Complex**: Complex number encoding

[Detailed Numeric Codecs API →](numeric-codecs.md)

## Packed Values Module

**Header**: `packed.hpp`

Zero-copy value types:

- **Packed<T, Codec>**: Single packed value
- **PackedVector<T>**: Dynamic array of packed values
- **PackedString**: Variable-length string
- **PackedArray<T, N>**: Fixed-size array
- **PackedList<T>**: Linked list

[Detailed Packed Types API →](packed.md)

## Algebraic Types Module

**Header**: `algebraic.hpp`

Type composition and sum/product types:

- **PackedOptional<T>**: Nullable value (Maybe)
- **PackedEither<L, R>**: Either left or right
- **PackedVariant<Ts...>**: Discriminated union
- **PackedProduct<Ts...>**: Tuple/struct
- **PackedResult<T, E>**: Success or error

[Detailed Algebraic Types API →](algebraic.md)

## STL Integration Module

**Header**: `stl_integration.hpp`

STL-compatible containers and iterators:

- **PackedContainer<T, Codec>**: STL-style container
- **PackedIterator<T, Codec>**: Random access iterator
- **Proxy references**: Decode-on-access
- **Standard algorithms**: sort, find, accumulate, etc.

[Detailed STL Integration API →](stl-integration.md)

## Algorithms Module

**Header**: `algorithms.hpp`

Generic algorithms for packed data:

- **packed_transform**: Transform elements
- **packed_filter**: Filter by predicate
- **packed_reduce**: Fold/accumulate
- **packed_map**: Map and collect
- **Parallel execution**: TBB support

[Detailed Algorithms API →](algorithms.md)

## Coordinates Module

**Header**: `coordinates.hpp`

Geometric types and transformations:

- **PackedVector2/3/4**: 2D/3D/4D vectors
- **PackedMatrix**: Matrices
- **PackedQuaternion**: Rotations
- **AffineTransform**: Transformations
- **Operations**: Dot, cross, matrix multiplication

[Detailed Coordinates API →](coordinates.md)

## Error Handling Module

**Header**: `error_handling.hpp`

Robust error handling:

- **Result<T, E>**: Success or error
- **Error types**: Structured error information
- **Error propagation**: Chainable operations
- **Pattern matching**: Visit for exhaustive handling

[Detailed Error Handling API →](error-handling.md)

## Compression Module

**Headers**: `huffman.hpp`, `lz77.hpp`, `arithmetic_coding.hpp`

Data compression algorithms:

- **Huffman coding**: Static and dynamic tables
- **LZ77**: Dictionary-based compression
- **Arithmetic coding**: Range encoding
- **Combined approaches**: Hybrid compression

[Detailed Compression API →](compression.md)

## CRC Module

**Header**: `crc.hpp`

Cyclic redundancy check:

- **CRC8**: 8-bit checksum
- **CRC16**: 16-bit checksum (CCITT, IBM)
- **CRC32**: 32-bit checksum (IEEE 802.3)
- **Incremental**: Update as data arrives

[Detailed CRC API →](crc.md)

## Common Types

### Size Types

```cpp
using size_t = std::size_t;         // Container sizes
using bit_size_t = uint64_t;        // Bit counts
```

### Concepts

```cpp
template<typename T>
concept BitSink = /* can write bits */;

template<typename T>
concept BitSource = /* can read bits */;

template<typename C, typename T>
concept Codec = /* can encode/decode T */;

template<typename T>
concept PackedValue = /* has encode/decode */;
```

## High-Level API

### Compression Functions

```cpp
// Compress a vector
template<typename Codec = EliasGamma, typename T>
std::vector<uint8_t> compress(const std::vector<T>& data);

// Decompress to vector
template<typename Codec = EliasGamma, typename T>
std::vector<T> decompress(std::span<const uint8_t> compressed);
```

### Convenience Constructors

```cpp
// Make packed optional
template<typename T>
auto make_optional(T value) -> PackedOptional<Packed<T>>;

// Make either left
template<typename Either>
auto make_left(typename Either::left_type value) -> Either;

// Make result ok
template<typename Result>
auto make_ok(typename Result::value_type value) -> Result;
```

## Type Aliases

Common type aliases for convenience:

```cpp
// Integers
using PackedInt = Packed<uint32_t, EliasGamma>;
using PackedLong = Packed<uint64_t, EliasDelta>;

// Floating point
using PackedFloat = Packed<float, Float32>;
using PackedDouble = Packed<double, Float64>;

// Strings
using PackedString = PackedVector<Packed<char, FixedWidth<8>>>;

// Collections
template<typename T>
using PackedVec = PackedVector<Packed<T, EliasGamma>>;
```

## Constants

```cpp
// Maximum encoded sizes
constexpr size_t MAX_ELIAS_GAMMA_BITS = 64;
constexpr size_t MAX_ELIAS_DELTA_BITS = 64;

// Special values
constexpr uint32_t INVALID_INDEX = ~0u;
```

## Next Steps

- Explore specific modules in detail
- Check out [examples](../examples/basic.md) for usage patterns
- Read [design principles](../guides/design-principles.md) for philosophy
