# Creating Custom Codecs

Learn how to extend PFC with your own codec implementations.

## Codec Requirements

A codec must implement:

```cpp
struct MyCodec {
    // Required: Is size fixed?
    static constexpr bool is_fixed_size();

    // Required: Maximum encoded bits
    static constexpr size_t max_encoded_bits();

    // Required: Encode value
    template<BitSink S>
    static void encode(ValueType value, S& sink);

    // Required: Decode value
    template<BitSource S>
    static ValueType decode(S& source);
};
```

## Example: Run-Length Encoding

```cpp
struct RunLengthCodec {
    static constexpr bool is_fixed_size() { return false; }
    static constexpr size_t max_encoded_bits() { return 128; }

    template<BitSink S>
    static void encode(uint32_t value, S& sink) {
        // Encode run length and value
        EliasGamma::encode(value & 0xFF, sink);  // Value
        EliasGamma::encode(value >> 8, sink);     // Count
    }

    template<BitSource S>
    static uint32_t decode(S& source) {
        uint32_t value = EliasGamma::decode(source);
        uint32_t count = EliasGamma::decode(source);
        return value | (count << 8);
    }
};
```

## Using Your Codec

Once defined, your codec works with all PFC infrastructure:

```cpp
// With packed values
using Packed RLE = Packed<uint32_t, RunLengthCodec>;

// With containers
PackedVector<PackedRLE> vec;

// With compression API
auto compressed = compress<RunLengthCodec>(data);
```

## Testing Your Codec

Always test round-trip correctness:

```cpp
TEST_CASE("RunLengthCodec round-trip") {
    uint8_t buffer[64];
    BitWriter writer(buffer);

    uint32_t original = 0x0102;
    RunLengthCodec::encode(original, writer);
    writer.align();

    BitReader reader(buffer);
    uint32_t decoded = RunLengthCodec::decode(reader);

    REQUIRE(decoded == original);
}
```

See [Design Principles](design-principles.md) for codec design philosophy.
