# PFC - Prefix-Free Codecs

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Header-Only](https://img.shields.io/badge/header--only-yes-green.svg)](https://github.com/queelius/pfc)
[![Tests](https://img.shields.io/badge/tests-689%20passing-brightgreen.svg)](https://github.com/queelius/pfc)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

*A header-only C++20 library for **zero-copy, prefix-free** data representations with full algebraic type support and STL integration.*

[Documentation](https://queelius.github.io/pfc/) | [API Reference](https://queelius.github.io/pfc/api/overview/) | [Examples](https://queelius.github.io/pfc/examples/)

## Features

- **Zero-copy invariant**: In-memory bytes = wire bytes (no marshaling)
- **Prefix-free codes**: Self-delimiting, streaming-friendly
- **Algebraic type system**: Sum types, product types, recursive types
- **STL integration**: Works seamlessly with standard algorithms
- **Rich codec library**: Universal codes, floating point, rationals, and more
- **Compile-time composition**: Modern C++ concepts and templates
- **Mathematical rigor**: Based on Stepanov's generic programming principles

## Quick Start

```cpp
#include <pfc/pfc.hpp>
using namespace pfc;

// Simple compression
std::vector<uint32_t> data = {1, 2, 3, 5, 8, 13, 21};
auto compressed = compress<EliasGamma>(data);
auto decompressed = decompress<EliasGamma, uint32_t>(compressed);

// Algebraic types
using OptionalInt = PackedOptional<Packed<int, Signed<EliasGamma>>>;
OptionalInt some_value = 42;
OptionalInt no_value = std::nullopt;

// STL integration
PackedContainer<uint32_t, EliasGamma> container;
container.push_back(100);
container.push_back(200);
std::sort(container.begin(), container.end());
```

## Building

```bash
mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=20
cmake --build .

# Run tests
./pfc_tests

# Run examples
./tutorial
```

## Test Coverage

The library is thoroughly tested with 689 passing assertions across 31 test cases:

- **Core Tests** (`pfc_tests`): 419 assertions - Codecs, packed values, basic algorithms
- **Advanced Tests** (`test_advanced`): 116 assertions - Algebraic types, STL integration, coordinates
- **Production Tests** (`test_production`): 142 assertions - Huffman, LZ77, error handling, allocators
- **CRC Tests** (`test_crc`): 12 assertions - CRC8, CRC16, CRC32 checksums
- **Stream/Arithmetic Tests**: Stream I/O and arithmetic coding

## Library Structure

### Core Components (`include/pfc/`)

- **`core.hpp`**: Bit I/O, concepts, fundamental abstractions
- **`codecs.hpp`**: Universal codes (Elias, Fibonacci, Rice), fixed-width
- **`numeric_codecs.hpp`**: Floating point, rationals, complex numbers
- **`packed.hpp`**: Packed value types and containers
- **`algebraic.hpp`**: Sum/product types, Maybe, Either, Result
- **`stl_integration.hpp`**: STL-compatible containers and iterators
- **`coordinates.hpp`**: Vectors, matrices, affine transformations
- **`algorithms.hpp`**: Generic algorithms for packed data
- **`error_handling.hpp`**: Result types for robust error handling
- **`huffman.hpp`**: Huffman coding with frequency analysis
- **`lz77.hpp`**: LZ77 compression algorithm
- **`crc.hpp`**: CRC8, CRC16, CRC32 checksums
- **`stream_io.hpp`**: Stream-based I/O operations
- **`arithmetic_coding.hpp`**: Arithmetic coding for compression
- **`allocator_support.hpp`**: Custom allocator support
- **`pfc.hpp`**: Main header including everything

### Codecs Available

#### Universal Integer Codes
- **Unary**: Simple unary encoding
- **Elias Gamma/Delta/Omega**: Asymptotically optimal universal codes
- **Fibonacci**: Based on Fibonacci sequence
- **Rice/Golomb**: Parametric codes for geometric distributions

#### Numeric Types
- **FloatingPoint**: Configurable mantissa/exponent (Float16, BFloat16, Float32, Float64)
- **FixedDecimal**: Exact decimal representation
- **Rational**: Fractions with continued fraction approximation
- **Complex**: Both rectangular and polar forms

#### Composite Types
- **Signed**: Zigzag encoding for signed integers
- **Optional/Maybe**: Nullable values
- **Either/Result**: Error handling
- **Vector/List**: Variable-length sequences
- **Variant**: Type-safe discriminated unions

## Design Principles

This library follows Alex Stepanov's generic programming principles:

1. **Regular types**: All types are copyable, assignable, and comparable
2. **Value semantics**: Objects behave like values, not references
3. **Separation of concerns**: Algorithms, containers, and codecs are orthogonal
4. **Zero-cost abstractions**: Template metaprogramming, no virtual dispatch
5. **Mathematical foundation**: Based on algebraic structures

## Advanced Usage

### Custom Codecs

```cpp
struct MyCodec {
    static constexpr bool is_fixed_size() { return false; }
    static constexpr size_t max_encoded_bits() { return 64; }

    template<BitSink S>
    static void encode(uint64_t value, S& sink) {
        // Your encoding logic
    }

    template<BitSource S>
    static uint64_t decode(S& source) {
        // Your decoding logic
    }
};
```

### Type-Erased Containers

```cpp
// Store different packed types in the same container
TypeErasedPackedContainer container;
container.push_back(PackedInt(42));
container.push_back(PackedRational(22, 7));
container.push_back(PackedString("hello"));
```

### Parallel Processing

```cpp
PackedContainer<double, FloatingPoint<16>> data(1000000);
// ... fill with data ...

// Parallel transform with TBB/execution policies
auto squared = packed_transform(std::execution::par_unseq,
    data.begin(), data.end(),
    [](double x) { return x * x; });
```

## Performance

The library achieves excellent compression ratios while maintaining zero-copy semantics:

- Geometric distributions: ~3.3 bits/value with Elias Gamma
- Sorted sequences: 65%+ improvement with delta encoding
- Floating point: Configurable precision/size tradeoff
- All operations are zero-copy with decode-on-access via proxies

## Documentation

Full documentation is available at [https://spinozarabel.github.io/pfc/](https://spinozarabel.github.io/pfc/)

- [Getting Started Guide](https://spinozarabel.github.io/pfc/guides/getting-started/)
- [API Reference](https://spinozarabel.github.io/pfc/api/overview/)
- [Usage Examples](https://spinozarabel.github.io/pfc/examples/)
- [Design Principles](https://spinozarabel.github.io/pfc/guides/design-principles/)

## Contributing

Contributions are welcome! The library is designed to be extended with:
- New codecs
- Additional algebraic types
- More generic algorithms
- Performance optimizations

See the [Contributing Guide](CONTRIBUTING.md) for details.

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Acknowledgments

Inspired by the work of Alex Stepanov, the design prioritizes mathematical elegance and efficiency. Special thanks to the authors of universal coding theory and the C++ standards committee for concepts and ranges.