# Best Practices

Guidelines for effective use of PFC in production code.

## Type Aliases

Create meaningful type aliases:

```cpp
// Good: Descriptive names
using UserId = Packed<uint32_t, EliasGamma>;
using UserName = PackedString;
using UserAge = Packed<uint8_t, FixedWidth<8>>;

// Better: Semantic types
struct User {
    UserId id;
    UserName name;
    UserAge age;
};
```

## Const Correctness

Use const for immutable data:

```cpp
void process(const PackedVector<PackedInt>& data) {
    // Can't modify data
    for (const auto& value : data) {
        std::cout << value << "\n";
    }
}
```

## Error Handling

Prefer Result types over exceptions:

```cpp
using ParseResult = PackedResult<PackedInt, PackedString>;

ParseResult parse(const std::string& input) {
    // Return Result instead of throwing
}
```

## Testing

Test codec round-trips thoroughly:

```cpp
TEST_CASE("Codec correctness") {
    for (uint32_t value = 0; value < 10000; ++value) {
        uint8_t buffer[64];
        BitWriter writer(buffer);
        MyCodec::encode(value, writer);
        writer.align();

        BitReader reader(buffer);
        auto decoded = MyCodec::decode(reader);
        REQUIRE(decoded == value);
    }
}
```

## Documentation

Document codec choices:

```cpp
// Using Rice<4> because our data follows geometric distribution
// with parameter ~16, making k=4 optimal
using PacketSize = Packed<uint32_t, Rice<4>>;
```

## Performance

Profile before optimizing:

```cpp
// Measure first
auto start = std::chrono::high_resolution_clock::now();
process_data(packed_vec);
auto end = std::chrono::high_resolution_clock::now();

// Then optimize if needed
```

## Code Organization

Separate concerns:

```
project/
  include/
    types.hpp        # Type definitions
    codecs.hpp       # Custom codecs
    serialization.hpp # Serialization logic
  src/
    main.cpp
  tests/
    test_codecs.cpp
```

See [Design Principles](design-principles.md) for philosophical guidance.
