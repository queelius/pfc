# Design Principles

PFC is built on a foundation of rigorous design principles inspired by Alex Stepanov's work on generic programming and the STL. Understanding these principles will help you use the library effectively and extend it correctly.

## The Zero-Copy Invariant

**Principle**: In-memory representation equals wire format.

Traditional serialization involves two copies:

```
Value -> Serialized Bytes -> Value
```

PFC eliminates this overhead:

```
PackedValue = Bytes (same representation)
```

### Benefits

1. **Performance**: No marshaling overhead
2. **Memory**: Single canonical representation
3. **Simplicity**: What you store is what you send

### Implementation

All packed types maintain bit-level representation:

```cpp
class PackedInt {
    std::vector<uint8_t> bytes_;  // Stored in encoded form

public:
    uint32_t get() const {
        BitReader reader(bytes_.data(), bytes_.size());
        return EliasGamma::decode(reader);  // Decode on access
    }
};
```

## Prefix-Free Codes

**Principle**: All encodings are self-delimiting.

A prefix-free code ensures no codeword is a prefix of another. This enables:

- Streaming without lookahead
- Concatenation without separators
- Guaranteed unique decoding

### Example

Elias Gamma encoding of integers:

```
1  -> 1
2  -> 010
3  -> 011
4  -> 00100
5  -> 00101
```

No codeword is a prefix of another, enabling unambiguous decoding:

```cpp
BitStream: 1 010 00100
Decodes to: [1, 2, 4]
```

## Regular Types

**Principle**: All types satisfy Stepanov's concept of regular types.

A regular type supports:

- Default construction
- Copy construction and assignment
- Move construction and assignment
- Equality comparison
- Destruction

### Why It Matters

Regular types can be:

- Stored in containers
- Passed by value
- Used with algorithms
- Reasoned about mathematically

### Implementation

```cpp
template<typename T, typename Codec>
class Packed {
public:
    Packed() = default;                           // Default construct
    Packed(const Packed&) = default;              // Copy
    Packed(Packed&&) noexcept = default;          // Move
    Packed& operator=(const Packed&) = default;   // Copy assign
    Packed& operator=(Packed&&) noexcept = default; // Move assign
    ~Packed() = default;                          // Destruct

    bool operator==(const Packed& other) const;   // Equality
};
```

## Separation of Concerns

**Principle**: Codecs, containers, and algorithms are orthogonal.

### Three Independent Dimensions

1. **Codecs**: How values are encoded
2. **Containers**: How collections are stored
3. **Algorithms**: How data is processed

### Composability

Any codec works with any container:

```cpp
// Same container, different codecs
PackedVector<Packed<uint32_t, EliasGamma>> vec1;
PackedVector<Packed<uint32_t, EliasDelta>> vec2;
PackedVector<Packed<uint32_t, Fibonacci>> vec3;

// Same codec, different containers
PackedVector<Packed<uint32_t, EliasGamma>> vec;
PackedList<Packed<uint32_t, EliasGamma>> list;
PackedArray<Packed<uint32_t, EliasGamma>, 10> arr;
```

## Concepts Over Inheritance

**Principle**: Use compile-time concepts instead of runtime polymorphism.

### Traditional OOP Approach

```cpp
class Codec {
public:
    virtual void encode(uint32_t value, BitSink& sink) = 0;
    virtual uint32_t decode(BitSource& source) = 0;
};

class EliasGamma : public Codec { /* ... */ };
```

Problems:

- Virtual dispatch overhead
- Dynamic allocation required
- Type erasure loses information

### PFC Approach

```cpp
template<typename C, typename T>
concept Codec = requires(T value, BitSink& sink, BitSource& source) {
    { C::encode(value, sink) } -> std::same_as<void>;
    { C::decode(source) } -> std::same_as<T>;
};

struct EliasGamma {
    template<BitSink S>
    static void encode(uint32_t value, S& sink) { /* ... */ }

    template<BitSource S>
    static uint32_t decode(S& source) { /* ... */ }
};
```

Benefits:

- Zero runtime overhead
- Better inlining
- Compile-time type checking
- No virtual tables

## Generic Programming

**Principle**: Algorithms work on any type satisfying requirements.

### Requirements as Concepts

```cpp
template<typename T>
concept PackedValue = requires(T t) {
    { t.encode() } -> std::convertible_to<std::vector<uint8_t>>;
    { T::decode(std::declval<std::span<const uint8_t>>()) } -> std::same_as<T>;
};

// Algorithm works on ANY PackedValue
template<PackedValue T>
size_t compute_size(const std::vector<T>& values) {
    return std::accumulate(values.begin(), values.end(), 0,
        [](size_t sum, const T& v) { return sum + v.byte_size(); });
}
```

## Value Semantics

**Principle**: Objects behave like mathematical values.

### Implications

1. **Copying** creates an independent object
2. **Equality** is based on value, not identity
3. **No aliasing** concerns
4. **Predictable** behavior

### Example

```cpp
PackedInt a(42);
PackedInt b = a;      // Independent copy
a = 100;              // Doesn't affect b
assert(b == 42);      // b retains original value
```

## Algebraic Types

**Principle**: Types form algebraic structures.

### Product Types (AND)

Combination of values:

```cpp
// Tuple: A × B × C
PackedProduct<PackedInt, PackedString, PackedFloat>
```

### Sum Types (OR)

Exclusive choice:

```cpp
// Variant: A | B | C
PackedVariant<PackedInt, PackedString, PackedFloat>
```

### Laws

Product and sum types obey algebraic laws:

```
A × 1 ≅ A           (unit)
A × B ≅ B × A       (commutativity)
(A × B) × C ≅ A × (B × C)  (associativity)

A + 0 ≅ A           (identity)
A + B ≅ B + A       (commutativity)
(A + B) + C ≅ A + (B + C)  (associativity)
```

## Proxy Iterators

**Principle**: Decode on access, never store decoded values.

### Why Proxies?

Storing decoded values violates the zero-copy invariant:

```cpp
// BAD: Stores decoded values
std::vector<uint32_t> decoded_values;

// GOOD: Decodes on access
PackedVector<Packed<uint32_t, EliasGamma>> packed_values;
```

### Implementation

```cpp
template<typename T, typename Codec>
class PackedIterator {
    const uint8_t* data_;
    size_t bit_offset_;

public:
    class Reference {
        const uint8_t* data_;
        size_t bit_offset_;

    public:
        operator T() const {
            BitReader reader(data_, bit_offset_);
            return Codec::decode(reader);  // Decode on conversion
        }
    };

    Reference operator*() const {
        return Reference{data_, bit_offset_};
    }
};
```

## Correctness Over Performance

**Principle**: Correctness first, then optimize.

### Testing Strategy

1. **Property-based testing**: Verify algebraic laws
2. **Round-trip testing**: encode(decode(x)) == x
3. **Edge cases**: Boundaries, empty, maximum values
4. **Fuzzing**: Random inputs

### Example Property

```cpp
TEST_CASE("Codec round-trip") {
    for (uint32_t value = 0; value < 10000; ++value) {
        uint8_t buffer[64];
        BitWriter writer(buffer);
        EliasGamma::encode(value, writer);
        writer.align();

        BitReader reader(buffer);
        auto decoded = EliasGamma::decode(reader);

        REQUIRE(decoded == value);
    }
}
```

## Extensibility

**Principle**: Make extension easy, modification unnecessary.

### Open/Closed Principle

The library is:

- **Open for extension**: Add new codecs, types, algorithms
- **Closed for modification**: Core doesn't need changes

### Example: Custom Codec

```cpp
struct MyCustomCodec {
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

// Immediately works with all PFC infrastructure
PackedVector<Packed<uint64_t, MyCustomCodec>> vec;
auto compressed = compress<MyCustomCodec>(data);
```

## Performance Characteristics

### Time Complexity

- **Encoding**: O(log n) for universal codes, O(1) for fixed-width
- **Decoding**: Same as encoding
- **Container access**: O(n) worst case (sequential scan)
- **Iteration**: O(n) for sequential, O(1) per element

### Space Complexity

- **Universal codes**: O(log n) bits per value
- **Fixed-width**: O(1) bits per value (constant)
- **Containers**: O(n) compressed bits, no overhead

## Mathematical Foundation

PFC is grounded in information theory and coding theory:

### Kraft-McMillan Inequality

For prefix-free codes:

```
∑ 2^(-li) ≤ 1
```

Where li is the length of the i-th codeword.

### Universal Codes

Elias codes achieve asymptotically optimal encoding for positive integers:

```
Elias Gamma:  ⌈log₂(n)⌉ + 2⌊log₂(n)⌋ + 1 bits
Elias Delta:  ⌈log₂(n)⌉ + 2⌊log₂(log₂(n))⌋ + 1 bits
Elias Omega:  O(log n + log log n + log log log n + ...)
```

## Lessons from STL

PFC adopts STL design patterns:

1. **Iterators**: Separate algorithms from containers
2. **Value semantics**: Predictable, composable
3. **Generic algorithms**: Work on ranges, not specific types
4. **Concepts**: Express requirements precisely
5. **No ownership**: Containers own, algorithms borrow

## Influences

- **Alex Stepanov**: Generic programming, STL design
- **Bartosz Milewski**: Category theory, algebraic types
- **Peter Elias**: Universal codes, information theory
- **Haskell/ML**: Algebraic data types, pattern matching
- **Rust**: Ownership, sum types, Result

## Further Reading

- Stepanov & McJones: "Elements of Programming"
- Elias: "Universal Codeword Sets and Representations of the Integers"
- Knuth: "The Art of Computer Programming, Vol 1"
- MacKay: "Information Theory, Inference, and Learning Algorithms"
