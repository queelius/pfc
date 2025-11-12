# Codecs API

Codecs are the foundation of PFC, providing transformations between values and bit representations. All codecs are stateless and use static methods for encoding/decoding.

## Codec Concept

A type `C` satisfies the `Codec<C, T>` concept if:

```cpp
template<typename C, typename T>
concept Codec = requires(T value, BitSink& sink, BitSource& source) {
    { C::encode(value, sink) } -> std::same_as<void>;
    { C::decode(source) } -> std::same_as<T>;
    { C::is_fixed_size() } -> std::convertible_to<bool>;
    { C::max_encoded_bits() } -> std::convertible_to<size_t>;
};
```

## Universal Integer Codes

### Unary

Simplest prefix-free code: n ones followed by a zero.

```cpp
struct Unary {
    template<BitSink S>
    static void encode(uint32_t value, S& sink);

    template<BitSource S>
    static uint32_t decode(S& source);

    static constexpr bool is_fixed_size() { return false; }
    static constexpr size_t max_encoded_bits() { return 32; }
};
```

**Encoding**:

- `0` → `0`
- `1` → `10`
- `2` → `110`
- `3` → `1110`

**Best for**: Very small values (0-10)

**Size**: `n + 1` bits for value `n`

### EliasGamma

Encodes `n` as unary(⌊log₂(n)⌋) followed by binary(n).

```cpp
struct EliasGamma {
    template<BitSink S>
    static void encode(uint32_t value, S& sink);

    template<BitSource S>
    static uint32_t decode(S& source);

    static constexpr bool is_fixed_size() { return false; }
    static constexpr size_t max_encoded_bits() { return 64; }
};
```

**Encoding**:

- `1` → `1`
- `2` → `010`
- `3` → `011`
- `4` → `00100`
- `5` → `00101`
- `8` → `0001000`

**Best for**: Small positive integers (1-1000)

**Size**: `2⌊log₂(n)⌋ + 1` bits

### EliasDelta

Improves on Gamma by encoding the length using Gamma.

```cpp
struct EliasDelta {
    template<BitSink S>
    static void encode(uint32_t value, S& sink);

    template<BitSource S>
    static uint32_t decode(S& source);

    static constexpr bool is_fixed_size() { return false; }
    static constexpr size_t max_encoded_bits() { return 64; }
};
```

**Best for**: Medium positive integers (1-1000000)

**Size**: `⌊log₂(n)⌋ + 2⌊log₂(⌊log₂(n)⌋)⌋ + 1` bits

### EliasOmega

Most efficient for large values.

```cpp
struct EliasOmega {
    template<BitSink S>
    static void encode(uint32_t value, S& sink);

    template<BitSource S>
    static uint32_t decode(S& source);

    static constexpr bool is_fixed_size() { return false; }
    static constexpr size_t max_encoded_bits() { return 64; }
};
```

**Best for**: Mixed small and large integers

**Size**: `O(log n + log log n + log log log n + ...)`

### Fibonacci

Encodes using Fibonacci representation with special terminator.

```cpp
struct Fibonacci {
    template<BitSink S>
    static void encode(uint32_t value, S& sink);

    template<BitSource S>
    static uint32_t decode(S& source);

    static constexpr bool is_fixed_size() { return false; }
    static constexpr size_t max_encoded_bits() { return 64; }
};
```

**Encoding** (Zeckendorf representation + `11` terminator):

- `1` → `11`
- `2` → `011`
- `3` → `0011`
- `4` → `1011`
- `5` → `00011`

**Best for**: Geometrically distributed values

**Size**: ~1.44 log₂(n) bits average

## Parametric Codes

### Rice

Optimized for geometric distributions with parameter k.

```cpp
template<unsigned K>
struct Rice {
    template<BitSink S>
    static void encode(uint32_t value, S& sink);

    template<BitSource S>
    static uint32_t decode(S& source);

    static constexpr bool is_fixed_size() { return false; }
    static constexpr size_t max_encoded_bits() { return 64; }
};
```

**Usage**:

```cpp
// For values with mean ~16, use k=4
using MyRice = Rice<4>;
MyRice::encode(42, sink);
```

**Encoding**: Quotient in unary, remainder in k bits

- `q = n >> k` (in unary)
- `r = n & ((1 << k) - 1)` (in k bits)

**Best for**: Values with known geometric parameter

### Golomb

Generalization of Rice to arbitrary divisors.

```cpp
template<unsigned M>
struct Golomb {
    template<BitSink S>
    static void encode(uint32_t value, S& sink);

    template<BitSource S>
    static uint32_t decode(S& source);

    static constexpr bool is_fixed_size() { return false; }
    static constexpr size_t max_encoded_bits() { return 64; }
};
```

**Usage**:

```cpp
using MyGolomb = Golomb<10>;  // Divisor = 10
MyGolomb::encode(42, sink);
```

## Fixed-Width Codes

### FixedWidth

Standard binary representation with N bits.

```cpp
template<unsigned N>
struct FixedWidth {
    template<BitSink S>
    static void encode(uint64_t value, S& sink);

    template<BitSource S>
    static uint64_t decode(S& source);

    static constexpr bool is_fixed_size() { return true; }
    static constexpr size_t max_encoded_bits() { return N; }
};
```

**Common aliases**:

```cpp
using Fixed8 = FixedWidth<8>;
using Fixed16 = FixedWidth<16>;
using Fixed32 = FixedWidth<32>;
using Fixed64 = FixedWidth<64>;
```

**Best for**: Uniformly distributed values, hashes

## Signed Integer Encodings

### Signed

Wraps any unsigned codec with zigzag encoding.

```cpp
template<typename UnsignedCodec>
struct Signed {
    template<BitSink S>
    static void encode(int32_t value, S& sink);

    template<BitSource S>
    static int32_t decode(S& source);

    static constexpr bool is_fixed_size() {
        return UnsignedCodec::is_fixed_size();
    }
    static constexpr size_t max_encoded_bits() {
        return UnsignedCodec::max_encoded_bits();
    }
};
```

**Zigzag mapping**:

```cpp
0  →  0
-1 →  1
1  →  2
-2 →  3
2  →  4
```

**Usage**:

```cpp
using SignedGamma = Signed<EliasGamma>;
SignedGamma::encode(-42, sink);

using SignedDelta = Signed<EliasDelta>;
PackedVector<Packed<int, SignedDelta>> vec;
```

## Codec Selection Guide

Choose codec based on your data distribution:

| Data Pattern | Best Codec | Reason |
|--------------|------------|--------|
| 0-10 | Unary | Minimal overhead |
| 1-1000 | EliasGamma | Good average case |
| 1-10^6 | EliasDelta | Better for larger values |
| Mixed sizes | EliasOmega | Adapts to magnitude |
| Geometric (many small, few large) | Fibonacci, Rice | Natural fit |
| Known parameter | Rice, Golomb | Optimized for distribution |
| Uniform | FixedWidth | Predictable size |
| Signed integers | Signed<Codec> | Efficient negative values |

## Usage Examples

### Basic Encoding/Decoding

```cpp
#include <pfc/codecs.hpp>

uint8_t buffer[64] = {};
BitWriter writer(buffer, sizeof(buffer));

// Encode values
EliasGamma::encode(42, writer);
EliasGamma::encode(100, writer);
EliasDelta::encode(1000000, writer);
writer.align();

// Decode
BitReader reader(buffer, sizeof(buffer));
auto v1 = EliasGamma::decode(reader);  // 42
auto v2 = EliasGamma::decode(reader);  // 100
auto v3 = EliasDelta::decode(reader);  // 1000000
```

### With Packed Values

```cpp
// Define packed types
using SmallInt = Packed<uint32_t, EliasGamma>;
using MediumInt = Packed<uint32_t, EliasDelta>;
using LargeInt = Packed<uint32_t, EliasOmega>;

// Use in containers
PackedVector<SmallInt> small_values;
small_values.push_back(1);
small_values.push_back(10);
small_values.push_back(100);
```

### Signed Values

```cpp
using SignedInt = Packed<int, Signed<EliasGamma>>;

SignedInt positive(42);
SignedInt negative(-42);
SignedInt zero(0);

// All efficiently encoded
PackedVector<SignedInt> vec = {-10, -5, 0, 5, 10};
```

### Parametric Codes

```cpp
// Audio residuals (small values common)
using AudioCodec = Rice<3>;
PackedVector<Packed<uint32_t, AudioCodec>> residuals;

// Image differences (parameter tuned to statistics)
using ImageCodec = Golomb<5>;
PackedVector<Packed<uint32_t, ImageCodec>> diffs;
```

## Performance Comparison

Typical sizes for value 1000:

| Codec | Bits | Size for 1000 |
|-------|------|---------------|
| Unary | n+1 | 1001 bits |
| EliasGamma | 2⌊log₂(n)⌋+1 | 19 bits |
| EliasDelta | ~log₂(n)+2log₂(log₂(n)) | 13 bits |
| EliasOmega | ~log₂(n) | 12 bits |
| Fibonacci | ~1.44log₂(n) | 15 bits |
| FixedWidth<32> | 32 | 32 bits |

## Creating Custom Codecs

To create your own codec:

```cpp
struct MyCodec {
    // Required: Is this fixed size?
    static constexpr bool is_fixed_size() { return false; }

    // Required: Maximum bits needed
    static constexpr size_t max_encoded_bits() { return 64; }

    // Required: Encode value
    template<BitSink S>
    static void encode(uint32_t value, S& sink) {
        // Your encoding logic
        sink.write_bit(value & 1);
        // ...
    }

    // Required: Decode value
    template<BitSource S>
    static uint32_t decode(S& source) {
        // Your decoding logic
        uint32_t value = source.read_bit();
        // ...
        return value;
    }
};
```

See [Creating Custom Codecs](../guides/custom-codecs.md) for detailed guide.

## See Also

- [Core API](core.md): BitWriter, BitReader
- [Numeric Codecs](numeric-codecs.md): Floating point, rationals
- [Packed Types](packed.md): Using codecs with values
- [Design Principles](../guides/design-principles.md): Why prefix-free codes
