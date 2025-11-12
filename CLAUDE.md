# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

PFC (Prefix-Free Codecs) is a header-only C++20 library implementing zero-copy, prefix-free data representations with full algebraic type support and STL integration. The library follows Alex Stepanov's generic programming principles.

## Commands

### Building the Project
```bash
# Configure and build
mkdir -p build && cd build
cmake .. -DCMAKE_CXX_STANDARD=20
cmake --build . -j4

# Build with misc examples
cmake .. -DBUILD_MISC_EXAMPLES=ON

# Run all tests
./pfc_tests                 # Core tests (codecs, packed values, basic algorithms)
./test_advanced             # Advanced features (algebraic types, STL integration)
./test_production           # Production features (error handling, allocators, Huffman, LZ77, CRC)
./test_stream_arithmetic    # Stream I/O and arithmetic coding

# Or use CTest
ctest                       # Run all tests
ctest -R pfc_tests         # Run specific test

# Run examples
./tutorial            # Basic usage tutorial
./advanced_demo       # Advanced features demo
./mini_demo          # Minimal demonstration
```

### Development Commands
```bash
# Run specific test sections using Catch2 tags
./pfc_tests "[codecs]"              # Test all codecs
./pfc_tests "[core]"                # Test bit I/O
./test_advanced "[algebraic]"       # Test algebraic types
./test_advanced "[stl]"             # Test STL integration
./test_production "[huffman]"       # Test Huffman coding
./test_stream_arithmetic "[stream]" # Test stream I/O

# List available test cases
./pfc_tests --list-tests
./test_advanced --list-tags

# Clean build
cd build && make clean

# Full rebuild
rm -rf build && mkdir build && cd build && cmake .. && cmake --build . -j4
```

## Architecture Overview

### Library Structure
```
include/pfc/
├── core.hpp              # Bit I/O (BitReader/BitWriter), concepts, fundamental abstractions
├── codecs.hpp            # Universal integer codes (Elias, Fibonacci, Rice, Unary, Fixed)
├── numeric_codecs.hpp    # Floating point, rational, complex, fixed decimal
├── packed.hpp            # Core packed value types and containers
├── algebraic.hpp         # Algebraic types (Sum, Product, Maybe, Either, Result)
├── stl_integration.hpp   # STL-compatible containers, iterators, and algorithms
├── coordinates.hpp       # Vectors, matrices, affine transformations
├── algorithms.hpp        # Generic algorithms for packed data
├── error_handling.hpp    # Error handling with Result types and exceptions
├── allocator_support.hpp # Custom allocator support for containers
├── huffman.hpp           # Huffman coding with static/dynamic tables
├── lz77.hpp              # LZ77 compression algorithm
├── crc.hpp               # CRC checksums (CRC32, etc.)
├── stream_io.hpp         # Stream-based I/O for serialization
├── arithmetic_coding.hpp # Arithmetic coding for near-optimal compression
└── pfc.hpp              # Main header (includes everything + high-level API)
```

### Key Design Principles

1. **Zero-Copy Invariant**: In-memory representation equals wire format
2. **Prefix-Free Codes**: Self-delimiting, no external metadata needed
3. **Compile-Time Composition**: Templates and concepts, no virtual dispatch
4. **Regular Types**: All types satisfy Stepanov's regular type requirements
5. **Orthogonal Design**: Codecs, containers, and algorithms are independent

### Core Concepts

```cpp
// Bit I/O
template<typename T>
concept BitSink = /* can write bits */;

template<typename T>
concept BitSource = /* can read bits */;

// Codecs
template<typename C, typename T>
concept Codec = /* transforms T to/from bits */;

// Packed Values
template<typename T>
concept PackedValue = /* has encode/decode static methods */;
```

### High-Level API (pfc.hpp)

The `pfc.hpp` header provides a simple compression API and utility functions:

```cpp
// Simple compression/decompression
auto compressed = compress<EliasGamma>(data);              // Compress vector
auto decompressed = decompress<uint32_t, EliasGamma>(compressed);  // Decompress

// Data analysis utilities
double entropy = calculate_entropy(data);                   // Calculate Shannon entropy
const char* codec = suggest_codec(sample);                  // Suggest best codec based on data

// Common type aliases available in pfc namespace
using U32 = PackedU32<EliasGamma>;
using U64 = PackedU64<EliasGamma>;
using I32 = PackedI32<SignedGamma>;
using I64 = PackedI64<SignedGamma>;
```

## Common Development Tasks

### Adding a New Codec
```cpp
// In include/pfc/codecs.hpp or numeric_codecs.hpp
struct MyNewCodec {
    static constexpr bool is_fixed_size() { return /* true/false */; }
    static constexpr size_t max_encoded_bits() { return /* max bits */; }

    template<BitSink S>
    static void encode(ValueType v, S& sink) {
        // Encoding logic
    }

    template<BitSource S>
    static ValueType decode(S& source) {
        // Decoding logic
    }
};
```

### Adding Tests
```cpp
// In tests/test_unified.cpp or test_advanced.cpp
TEST_CASE("MyNewCodec", "[codecs]") {
    SECTION("Round-trip") {
        uint8_t buffer[64] = {};
        BitWriter writer(buffer);
        MyNewCodec::encode(test_value, writer);
        writer.align();

        BitReader reader(buffer);
        auto decoded = MyNewCodec::decode(reader);
        REQUIRE(decoded == test_value);
    }
}
```

### Creating Algebraic Types
```cpp
// Compose existing types
using MyOptional = PackedOptional<PackedInt>;
using MyEither = PackedEither<PackedInt, PackedString>;
using MyList = PackedVector<PackedRational>;
```

## Testing Strategy

The test suite uses Catch2 framework with organized sections:

- **test_unified.cpp**: Core functionality
  - Bit I/O operations (BitReader/BitWriter)
  - All codec implementations (Elias, Fibonacci, Rice, Fixed, etc.)
  - Packed values and containers
  - Basic algorithms

- **test_advanced.cpp**: Advanced features
  - Algebraic types (PackedOptional, PackedEither, PackedVariant)
  - STL integration (containers, iterators, algorithms)
  - Coordinate systems (vectors, matrices, transformations)

- **test_production.cpp**: Production-ready features
  - Error handling and Result types
  - Custom allocator support
  - Huffman coding
  - LZ77 compression
  - CRC checksums

- **test_stream_arithmetic.cpp**: Advanced compression
  - Stream-based I/O
  - Arithmetic coding

Focus on round-trip correctness, edge cases, and random test data. Use Catch2 sections and tags for organization.

## Performance Considerations

1. **Proxy Iterators**: Decode on access, never store decoded values
2. **Type Erasure**: Optional, only when runtime polymorphism needed
3. **Parallel Algorithms**: Use execution policies when available
4. **Memory Layout**: Bit-packed, minimal padding only at container end

## Integration with Related Libraries

### packed_data library
Located at `/home/spinoza/github/repos/packed_data/`
- More comprehensive container abstractions
- Variable-length arrays with O(1) access
- Bit matrix implementations

### universal_coders library
Located at `/home/spinoza/github/repos/universal_coders/`
- Additional universal coding schemes
- Experimental codecs

## Known Issues

1. Some advanced tests have precision issues with floating point comparisons
2. The advanced_demo may have infinite loops in certain sections - use with caution
3. Arithmetic coding and stream I/O are experimental and may need additional validation

## Code Style Guidelines

1. Use concepts for all template requirements
2. Prefer static polymorphism over virtual functions
3. Follow STL naming conventions
4. Keep headers self-contained
5. Maintain zero-copy invariant in all operations