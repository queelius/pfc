# Quick Start Guide

This guide will get you up and running with PFC in minutes. We'll cover the essential concepts and common usage patterns.

## Basic Compression

The simplest way to use PFC is with the high-level compression API:

```cpp
#include <pfc/pfc.hpp>
#include <vector>

using namespace pfc;

int main() {
    // Create some data
    std::vector<uint32_t> data = {1, 2, 3, 5, 8, 13, 21, 34};

    // Compress with Elias Gamma coding (default)
    auto compressed = compress(data);

    // Decompress
    auto decompressed = decompress<uint32_t>(compressed);

    // Verify
    assert(data == decompressed);
}
```

## Understanding Codecs

Codecs transform values to and from bit representations. PFC provides many codecs for different data distributions:

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

// Small positive integers (1-1000)
auto compressed_gamma = compress<EliasGamma>(data);

// Mix of small and large values
auto compressed_delta = compress<EliasDelta>(data);

// Geometrically distributed (many small, few large)
auto compressed_fib = compress<Fibonacci>(data);

// Known range with parameter
auto compressed_rice = compress<Rice<4>>(data);  // Parameter k=4
```

## Packed Values

Packed values maintain the zero-copy invariant - they're stored in compressed form:

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

// Define a packed integer type using Elias Gamma
using PackedInt = Packed<uint32_t, EliasGamma>;

// Values are stored compressed
PackedInt value(42);

// Decode on access (zero-copy)
uint32_t decoded = value.get();

// Can be used in containers
std::vector<PackedInt> values;
values.push_back(PackedInt(10));
values.push_back(PackedInt(20));
```

## Packed Containers

For dynamic collections, use PackedVector:

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

// Create a packed vector
PackedVector<Packed<uint32_t, EliasGamma>> vec;

// Add elements
vec.push_back(1);
vec.push_back(2);
vec.push_back(3);

// Access elements (decoded on demand)
for (const auto& value : vec) {
    std::cout << value << "\n";
}

// Works with algorithms
auto sum = std::accumulate(vec.begin(), vec.end(), 0u);
```

## Algebraic Types

PFC supports rich type composition:

### Optional Values

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

// Define an optional packed integer
using OptionalInt = PackedOptional<Packed<int, Signed<EliasGamma>>>;

OptionalInt some_value(42);
OptionalInt none_value(std::nullopt);

// Check and access
if (some_value) {
    std::cout << "Value: " << *some_value << "\n";
}

// Pattern matching with visit
some_value.visit(overloaded{
    [](const auto& val) { std::cout << "Some: " << val << "\n"; },
    []() { std::cout << "None\n"; }
});
```

### Either Type (Tagged Union)

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

// Either holds one of two types
using IntOrString = PackedEither<
    Packed<int, Signed<EliasGamma>>,
    PackedString
>;

IntOrString left = make_left<IntOrString>(42);
IntOrString right = make_right<IntOrString>("hello");

// Pattern matching
left.visit(overloaded{
    [](int i) { std::cout << "Int: " << i << "\n"; },
    [](const std::string& s) { std::cout << "String: " << s << "\n"; }
});
```

### Result Type (Error Handling)

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

// Result type for operations that can fail
using Result = PackedResult<
    Packed<int, Signed<EliasGamma>>,  // Success type
    PackedString                       // Error type
>;

Result parse_int(const std::string& s) {
    try {
        return make_ok<Result>(std::stoi(s));
    } catch (...) {
        return make_err<Result>("Invalid integer");
    }
}

// Check result
auto result = parse_int("42");
if (result.is_ok()) {
    std::cout << "Parsed: " << result.unwrap() << "\n";
} else {
    std::cerr << "Error: " << result.unwrap_err() << "\n";
}
```

## Numeric Types

### Floating Point

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

// Different precisions
using Float16 = FloatingPoint<11, 5>;  // Half precision
using Float32 = FloatingPoint<24, 8>;  // Single precision
using Float64 = FloatingPoint<53, 11>; // Double precision

PackedVector<Packed<double, Float16>> compressed_floats;
compressed_floats.push_back(3.14159);
compressed_floats.push_back(2.71828);
```

### Rationals

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

// Exact rational representation
PackedRational pi_approx(22, 7);  // 22/7
PackedRational euler_approx(271828, 100000);  // 2.71828

double as_double = pi_approx.to_double();
```

## Working with Bit I/O

For low-level control, use BitWriter and BitReader:

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

// Writing
uint8_t buffer[64] = {};
BitWriter writer(buffer, sizeof(buffer));

// Encode values
EliasGamma::encode(42, writer);
EliasGamma::encode(100, writer);
writer.align();  // Align to byte boundary

// Reading
BitReader reader(buffer, sizeof(buffer));
auto value1 = EliasGamma::decode(reader);
auto value2 = EliasGamma::decode(reader);

assert(value1 == 42 && value2 == 100);
```

## STL Integration

PFC containers work seamlessly with STL algorithms:

```cpp
#include <pfc/pfc.hpp>
#include <algorithm>
#include <numeric>

using namespace pfc;

PackedVector<Packed<int, Signed<EliasGamma>>> vec = {5, 2, 8, 1, 9};

// Sorting
std::sort(vec.begin(), vec.end());

// Finding
auto it = std::find(vec.begin(), vec.end(), 8);

// Accumulation
int sum = std::accumulate(vec.begin(), vec.end(), 0);

// Transformation
PackedVector<Packed<int, Signed<EliasGamma>>> squared;
std::transform(vec.begin(), vec.end(),
               std::back_inserter(squared),
               [](int x) { return x * x; });
```

## Choosing the Right Codec

Different codecs excel with different data distributions:

| Codec | Best For | Example Use Case |
|-------|----------|------------------|
| Unary | Very small values (1-10) | Boolean-heavy data |
| Elias Gamma | Small positive integers | Counts, small IDs |
| Elias Delta | Medium values | General purpose |
| Elias Omega | Mixed small/large | Variable-size data |
| Fibonacci | Geometric distribution | Network packet sizes |
| Rice/Golomb | Known distribution parameter | Audio/image residuals |
| Fixed Width | Uniform distribution | Random data, hashes |

## Performance Tips

1. **Choose the right codec** for your data distribution
2. **Batch operations** when possible to amortize overhead
3. **Use packed containers** instead of std::vector<Packed<T>>
4. **Profile first** - zero-copy doesn't mean zero-cost for small data
5. **Consider alignment** - align bit streams for better cache usage

## Common Patterns

### Configuration Storage

```cpp
struct Config {
    uint32_t version;
    uint32_t feature_flags;
    std::string name;
};

// Serialize
PackedProduct<
    Packed<uint32_t, EliasGamma>,
    Packed<uint32_t, FixedWidth<32>>,
    PackedString
> packed_config(42, 0xFF00, "MyApp");

// Save to file
std::ofstream file("config.dat", std::ios::binary);
file.write(packed_config.data(), packed_config.byte_size());
```

### Network Protocol

```cpp
// Define message types
using Request = PackedVariant<
    PackedString,                      // GetRequest
    Packed<uint32_t, EliasGamma>       // SetRequest
>;

using Response = PackedResult<
    PackedString,                      // Success
    Packed<uint32_t, FixedWidth<8>>    // Error code
>;

// Send request
Request req = make_variant<0, Request>("key");
send_over_network(req.data(), req.byte_size());

// Receive response
Response resp = receive_response();
```

## Next Steps

- [API Reference](../api/overview.md): Detailed documentation
- [Examples](../examples/basic.md): More comprehensive examples
- [Custom Codecs](custom-codecs.md): Create your own encodings
- [Design Principles](design-principles.md): Understand the philosophy
