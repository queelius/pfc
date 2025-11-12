# Core API

The core module provides fundamental building blocks for bit-level I/O and the conceptual foundation of the library.

## Header

```cpp
#include <pfc/core.hpp>
```

## BitWriter

Writes bits to a byte buffer with efficient bit packing.

### Interface

```cpp
class BitWriter {
public:
    // Constructors
    BitWriter(uint8_t* buffer, size_t capacity);
    BitWriter(std::vector<uint8_t>& buffer);
    BitWriter(std::span<uint8_t> buffer);

    // Write operations
    void write_bit(bool bit);
    void write_bits(uint64_t value, size_t num_bits);

    // Alignment
    void align();  // Align to next byte boundary
    void pad_to_byte();  // Pad current byte with zeros

    // State queries
    size_t bits_written() const;
    size_t bytes_written() const;
    size_t capacity() const;
    bool has_space(size_t num_bits) const;

    // Access
    const uint8_t* data() const;
    uint8_t* data();
};
```

### Usage

```cpp
uint8_t buffer[64] = {};
BitWriter writer(buffer, sizeof(buffer));

// Write individual bits
writer.write_bit(true);
writer.write_bit(false);

// Write multiple bits
writer.write_bits(0b1010, 4);  // Write 4 bits

// Check space
if (writer.has_space(32)) {
    writer.write_bits(42, 32);
}

// Align to byte boundary
writer.align();

std::cout << "Wrote " << writer.bits_written() << " bits ("
          << writer.bytes_written() << " bytes)\n";
```

## BitReader

Reads bits from a byte buffer.

### Interface

```cpp
class BitReader {
public:
    // Constructors
    BitReader(const uint8_t* buffer, size_t size);
    BitReader(std::span<const uint8_t> buffer);
    BitReader(const std::vector<uint8_t>& buffer);

    // Read operations
    bool read_bit();
    uint64_t read_bits(size_t num_bits);

    // Alignment
    void align();  // Skip to next byte boundary

    // State queries
    size_t bits_read() const;
    size_t bytes_read() const;
    size_t remaining_bits() const;
    size_t remaining_bytes() const;
    bool has_bits(size_t num_bits) const;
    bool at_end() const;

    // Positioning
    void skip_bits(size_t num_bits);
    void reset();  // Reset to beginning
};
```

### Usage

```cpp
BitReader reader(buffer, size);

// Read individual bits
bool b1 = reader.read_bit();
bool b2 = reader.read_bit();

// Read multiple bits
uint32_t value = reader.read_bits(32);

// Check availability
if (reader.has_bits(16)) {
    uint16_t short_val = reader.read_bits(16);
}

// Skip bits
reader.skip_bits(8);

// Check if done
if (reader.at_end()) {
    std::cout << "Finished reading\n";
}
```

## Concepts

### BitSink

Types that can accept bit writes.

```cpp
template<typename T>
concept BitSink = requires(T sink, bool bit, uint64_t value, size_t n) {
    { sink.write_bit(bit) } -> std::same_as<void>;
    { sink.write_bits(value, n) } -> std::same_as<void>;
    { sink.align() } -> std::same_as<void>;
};
```

**Models**: BitWriter, any type with compatible interface

### BitSource

Types that can provide bit reads.

```cpp
template<typename T>
concept BitSource = requires(T source, size_t n) {
    { source.read_bit() } -> std::convertible_to<bool>;
    { source.read_bits(n) } -> std::convertible_to<uint64_t>;
    { source.align() } -> std::same_as<void>;
    { source.at_end() } -> std::convertible_to<bool>;
};
```

**Models**: BitReader, any type with compatible interface

### Codec

Types that can encode and decode values.

```cpp
template<typename C, typename T>
concept Codec = requires(T value) {
    { C::encode(value, std::declval<BitSink&>()) } -> std::same_as<void>;
    { C::decode(std::declval<BitSource&>()) } -> std::same_as<T>;
    { C::is_fixed_size() } -> std::convertible_to<bool>;
    { C::max_encoded_bits() } -> std::convertible_to<size_t>;
};
```

### PackedValue

Types that maintain zero-copy invariant.

```cpp
template<typename T>
concept PackedValue = requires(T value) {
    { value.byte_size() } -> std::convertible_to<size_t>;
    { value.data() } -> std::convertible_to<const uint8_t*>;
    { T::decode(std::declval<std::span<const uint8_t>>()) } -> std::same_as<T>;
};
```

## Utility Functions

### Bit Manipulation

```cpp
// Count leading zeros
constexpr uint32_t clz(uint32_t x);
constexpr uint64_t clz(uint64_t x);

// Count trailing zeros
constexpr uint32_t ctz(uint32_t x);
constexpr uint64_t ctz(uint64_t x);

// Population count (number of 1 bits)
constexpr uint32_t popcount(uint32_t x);
constexpr uint64_t popcount(uint64_t x);

// Bit length (position of highest bit)
constexpr uint32_t bit_length(uint32_t x);
constexpr uint64_t bit_length(uint64_t x);
```

### Alignment

```cpp
// Round up to next byte boundary
constexpr size_t align_to_byte(size_t bit_count);

// Check if aligned
constexpr bool is_byte_aligned(size_t bit_count);
```

## Error Handling

Most core operations are noexcept, but may trigger assertions in debug builds:

```cpp
// Debug assertions
assert(writer.has_space(num_bits));  // Writing without space
assert(reader.has_bits(num_bits));   // Reading past end
assert(num_bits <= 64);              // Too many bits requested
```

## Performance

- BitWriter/BitReader use efficient bitwise operations
- No dynamic allocation (operate on provided buffers)
- Branch-free bit manipulation where possible
- Optimized for sequential access patterns

### Benchmarks

Typical performance on modern CPU:

- write_bit: ~1-2 ns
- read_bit: ~1-2 ns
- write_bits(32): ~2-3 ns
- read_bits(32): ~2-3 ns

## Examples

### Round-Trip Encoding

```cpp
#include <pfc/core.hpp>

uint8_t buffer[64];
BitWriter writer(buffer, sizeof(buffer));

// Encode some data
writer.write_bits(0xDEADBEEF, 32);
writer.write_bit(true);
writer.write_bits(0xFF, 8);
writer.align();

// Decode
BitReader reader(buffer, writer.bytes_written());
uint32_t value1 = reader.read_bits(32);  // 0xDEADBEEF
bool flag = reader.read_bit();            // true
uint8_t byte = reader.read_bits(8);      // 0xFF
```

### Custom BitSink

```cpp
class FileWriter {
    std::ofstream file_;
    uint8_t byte_buffer_;
    size_t bit_count_;

public:
    void write_bit(bool bit) {
        byte_buffer_ |= (bit << bit_count_);
        if (++bit_count_ == 8) {
            file_.write(reinterpret_cast<char*>(&byte_buffer_), 1);
            byte_buffer_ = 0;
            bit_count_ = 0;
        }
    }

    void write_bits(uint64_t value, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            write_bit((value >> i) & 1);
        }
    }

    void align() {
        if (bit_count_ > 0) {
            file_.write(reinterpret_cast<char*>(&byte_buffer_), 1);
            byte_buffer_ = 0;
            bit_count_ = 0;
        }
    }
};

static_assert(BitSink<FileWriter>);
```

## See Also

- [Codecs API](codecs.md): Using BitWriter/BitReader with codecs
- [Packed Types](packed.md): High-level packed value types
- [Design Principles](../guides/design-principles.md): Zero-copy invariant
