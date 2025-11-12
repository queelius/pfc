# PFC - Prefix-Free Codecs

Welcome to the documentation for **PFC**, a header-only C++20 library for zero-copy, prefix-free data representations with full algebraic type support and STL integration.

## Overview

PFC implements a comprehensive suite of prefix-free codecs and data structures that maintain a zero-copy invariant: the in-memory representation is identical to the wire format. This eliminates marshaling overhead while providing powerful compression and type composition capabilities.

## Key Features

- **Zero-Copy Invariant**: In-memory bytes = wire bytes (no marshaling)
- **Prefix-Free Codes**: Self-delimiting, streaming-friendly encodings
- **Algebraic Type System**: Sum types, product types, recursive types
- **STL Integration**: Works seamlessly with standard algorithms
- **Rich Codec Library**: Universal codes, floating point, rationals, and more
- **Compile-Time Composition**: Modern C++ concepts and templates
- **Mathematical Rigor**: Based on Stepanov's generic programming principles

## Test Coverage

The library is thoroughly tested with **689 passing assertions** across **31 test cases**:

- Core Tests (419 assertions): Codecs, packed values, basic algorithms
- Advanced Tests (116 assertions): Algebraic types, STL integration, coordinates
- Production Tests (142 assertions): Huffman, LZ77, error handling, allocators
- CRC Tests (12 assertions): CRC8, CRC16, CRC32 checksums
- Stream/Arithmetic Tests: Stream I/O and arithmetic coding

## Quick Example

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

## Why PFC?

### Zero-Copy Performance

Traditional serialization libraries copy data between in-memory and wire representations. PFC eliminates this overhead by making them identical:

```cpp
// Traditional approach (2 copies)
std::vector<int> data = {1, 2, 3};
auto serialized = serialize(data);  // Copy 1
send_over_network(serialized);
auto received = receive_from_network();
auto deserialized = deserialize<std::vector<int>>(received);  // Copy 2

// PFC approach (0 copies)
PackedVector<PackedInt> data = {1, 2, 3};
send_over_network(data.data(), data.byte_size());  // Direct memory access
PackedVector<PackedInt> received(buffer, buffer_size);  // Zero-copy view
```

### Prefix-Free Advantage

Prefix-free codes are self-delimiting, enabling:

- Streaming processing without lookahead
- Concatenation without separators
- Variable-length encoding with guaranteed decoding

### Type Safety

Strong typing prevents common serialization errors:

```cpp
// Type-safe tagged unions
using Response = PackedVariant<
    Packed<Success, SuccessCodec>,
    Packed<Error, ErrorCodec>
>;

// Compiler enforces exhaustive pattern matching
auto result = response.visit(overloaded{
    [](const Success& s) { return process(s); },
    [](const Error& e) { return handle_error(e); }
});
```

## Architecture

PFC is organized into focused modules:

- **Core**: Bit I/O primitives, concepts, fundamental abstractions
- **Codecs**: Universal integer codes (Elias, Fibonacci, Rice)
- **Numeric**: Floating point, rational, complex number codecs
- **Packed**: Value types and containers with zero-copy semantics
- **Algebraic**: Sum and product types for type composition
- **STL Integration**: Iterators, algorithms, and standard compatibility
- **Compression**: Huffman, LZ77, arithmetic coding
- **Utilities**: CRC checksums, stream I/O, error handling

## Next Steps

- [Installation Guide](guides/installation.md): Get up and running
- [Quick Start Tutorial](guides/quick-start.md): Learn the basics
- [API Reference](api/overview.md): Detailed documentation
- [Examples](examples/basic.md): Real-world usage patterns
- [Design Principles](guides/design-principles.md): Understand the philosophy

## Requirements

- C++20 compliant compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.15+ (for building tests and examples)
- Header-only library, no external dependencies required

## License

PFC is released under the MIT License. See [LICENSE](https://github.com/spinozarabel/pfc/blob/main/LICENSE) for details.
