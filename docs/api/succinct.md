# Succinct Data Structures

The `succinct.hpp` header provides space-efficient data structures that support powerful query operations in constant or logarithmic time while maintaining the zero-copy invariant.

## Overview

Succinct data structures achieve near-optimal space complexity (n + o(n) bits) while providing efficient query operations. The PFC library implements these structures with its signature zero-copy architecture: wire format equals in-memory format.

## SuccinctBitVector

A space-efficient bit vector supporting fast rank and select operations.

### Space Complexity

- **Bit data**: n bits (exact)
- **Rank support**: ~0.2% overhead (n/512 + n/65536 bits)
- **Total**: n + 0.2%n bits

### Time Complexity

| Operation | Complexity | Description |
|-----------|------------|-------------|
| `operator[]` | O(1) | Access bit at position |
| `rank(pos)` | O(1) | Count 1s in [0, pos) |
| `select(k)` | O(log n) | Find position of k-th 1 |
| `set/reset/flip` | O(1) | Modify individual bits |
| `build_rank_support()` | O(n) | Build rank/select metadata |

### Construction

```cpp
#include <pfc/succinct.hpp>
using namespace pfc::succinct;

// Create bit vector with 1000 bits
SuccinctBitVector bv(1000);

// Set some bits
for (size_t i = 0; i < 1000; i += 7) {
    bv.set(i);  // Set every 7th bit
}

// Build rank/select support (required for rank/select operations)
bv.build_rank_support();
```

### Query Operations

```cpp
// Access individual bits
bool bit = bv[42];  // O(1) access

// Rank: count number of 1s before position
size_t ones_before_100 = bv.rank(100);  // Count 1s in [0, 100)

// Select: find position of k-th 1
size_t tenth_one = bv.select(10);  // Find position of 10th 1-bit

// Rank at end gives total count
size_t total_ones = bv.rank(bv.size());
```

### Zero-Copy Serialization

The key advantage of PFC's succinct structures is instant serialization/deserialization:

```cpp
// Serialize to buffer
uint8_t buffer[10000];
BitWriter writer(buffer);
SuccinctBitVector::encode(bv, writer);
writer.align();

// Deserialize from buffer (zero-copy!)
BitReader reader(buffer, 10000);
auto bv2 = SuccinctBitVector::decode(reader);

// Works immediately - no build step needed!
size_t rank = bv2.rank(100);  // Instant query
```

**Traditional Approach:**
```
Wire Format → Parse → Build Metadata → Query Ready
  (compact)    (slow)    (slow)          (usable)
```

**PFC Approach:**
```
Wire Format → Query Ready
  (compact)     (instant!)
```

### Block-Based Rank Support

SuccinctBitVector uses a two-level indexing structure for O(1) rank queries:

#### Architecture

```
Superblock (65536 bits = 8KB)
├── Block (512 bits)
├── Block (512 bits)
├── Block (512 bits)
└── ...

Metadata:
- Superblocks: Absolute rank every 65536 bits (32-bit integer)
- Blocks: Relative rank within superblock (16-bit integer)
```

#### Space Overhead Calculation

```
Superblock overhead: 32 bits / 65536 bits = 0.049%
Block overhead: 16 bits / 512 bits = 3.125%
Average: (65536 / 65536) * 0.049% + (512 / 512) * 3.125% / 128 = 0.024%
Total overhead: ~0.2% (including alignment)
```

### Modification Operations

```cpp
SuccinctBitVector bv(100);

// Set bits
bv.set(42);        // Set bit 42 to 1
bv.set(43, true);  // Explicit set to 1
bv.set(44, false); // Set to 0 (same as reset)

// Reset bits
bv.reset(42);      // Set bit 42 to 0

// Flip bits
bv.flip(42);       // Toggle bit 42

// Note: After modifications, rebuild rank support
bv.build_rank_support();
```

### Size and Capacity

```cpp
SuccinctBitVector bv(1000);

size_t n = bv.size();           // Number of bits (1000)
size_t bytes = bv.byte_size();  // Memory size in bytes
bool empty = bv.empty();        // Check if size is 0
```

### API Reference

#### Constructor

```cpp
SuccinctBitVector(size_t num_bits, bool initial_value = false);
```

Creates a bit vector with `num_bits` bits, all initialized to `initial_value`.

#### Bit Access

```cpp
bool operator[](size_t pos) const;
```

Returns the value of the bit at position `pos`. O(1) time.

#### Bit Modification

```cpp
void set(size_t pos, bool value = true);
void reset(size_t pos);
void flip(size_t pos);
```

- `set(pos, value)`: Set bit at position to value (default true)
- `reset(pos)`: Set bit at position to false
- `flip(pos)`: Toggle bit at position

Note: Rank support must be rebuilt after modifications.

#### Rank Support

```cpp
void build_rank_support();
```

Builds the rank/select metadata structure. Must be called after construction or modification and before using `rank()` or `select()` operations. O(n) time, 0.2% space overhead.

#### Rank Operation

```cpp
size_t rank(size_t pos) const;
```

Returns the number of 1-bits in the range [0, pos). O(1) time.

Special cases:
- `rank(0)` returns 0
- `rank(size())` returns total count of 1s
- `rank(pos)` where `pos > size()` returns total count of 1s

#### Select Operation

```cpp
size_t select(size_t k) const;
```

Returns the position of the k-th 1-bit (0-indexed). O(log n) time using binary search.

Returns `size_t(-1)` if fewer than k+1 ones exist.

#### Size Operations

```cpp
size_t size() const;
size_t byte_size() const;
bool empty() const;
```

- `size()`: Number of bits in the vector
- `byte_size()`: Memory footprint in bytes
- `empty()`: True if size is 0

#### Zero-Copy Encoding/Decoding

```cpp
template<BitSink S>
static void encode(const SuccinctBitVector& bv, S& sink);

template<BitSource S>
static SuccinctBitVector decode(S& source);
```

Encodes or decodes a SuccinctBitVector to/from a bit stream. The decoded structure is immediately queryable without any build step.

## Use Cases

### 1. Sparse Sets

Represent sparse sets of integers efficiently:

```cpp
// Represent set {7, 14, 21, 28, ...} in universe [0, 1000)
SuccinctBitVector set(1000);
for (size_t i = 7; i < 1000; i += 7) {
    set.set(i);
}
set.build_rank_support();

// Check membership
bool contains_42 = set[42];  // false
bool contains_49 = set[49];  // true

// Count elements before position
size_t elements_before_100 = set.rank(100);  // 14 elements

// Find k-th element
size_t fifth_element = set.select(4);  // Position 28 (0-indexed)
```

### 2. Bitmap Indices

Efficient filtering in databases:

```cpp
// Index for "active users" in database of 1M users
SuccinctBitVector active_users(1000000);

// Mark active users
for (auto user_id : active_user_ids) {
    active_users.set(user_id);
}
active_users.build_rank_support();

// Query: How many active users have ID < 500000?
size_t count = active_users.rank(500000);

// Query: What is the 1000th active user ID?
size_t user_id = active_users.select(999);
```

### 3. Compressed Graphs

Building block for succinct graph representations:

```cpp
// Mark which edges exist in a graph
// For n vertices, adjacency represented as n^2 bit vector
size_t n = 1000;
SuccinctBitVector adjacency(n * n);

// Add edges
auto add_edge = [&](size_t u, size_t v) {
    adjacency.set(u * n + v);
};

add_edge(0, 1);
add_edge(0, 5);
// ... more edges ...

adjacency.build_rank_support();

// Query: How many edges from vertex 0?
size_t degree = adjacency.rank((0 + 1) * n) - adjacency.rank(0 * n);
```

### 4. Zero-Copy Persistence

Instant loading of pre-built data structures:

```cpp
// Server startup: load pre-built bit vector from file
FILE* fp = fopen("index.pfc", "rb");
std::vector<uint8_t> data = read_all(fp);

BitReader reader(data.data(), data.size());
auto index = SuccinctBitVector::decode(reader);

// Immediately usable - no parsing or build time!
size_t count = index.rank(position);
```

## Performance Characteristics

### Memory Layout

```
┌─────────────────────────────────────────────────────┐
│ Bit Vector Data (n bits)                            │
├─────────────────────────────────────────────────────┤
│ Superblock Ranks (32-bit integers, n/65536 entries)│
├─────────────────────────────────────────────────────┤
│ Block Ranks (16-bit integers, n/512 entries)        │
└─────────────────────────────────────────────────────┘

Total: n + (n/65536)*32 + (n/512)*16 bits
     = n + 0.049%n + 3.125%n bits
     ≈ n + 0.2%n bits (with alignment)
```

### Cache Efficiency

- **Block size**: 512 bits = 64 bytes (one cache line)
- **Rank query**: At most 2 cache line accesses (1 superblock + 1 block)
- **Sequential access**: Excellent cache locality

### Comparison with Alternatives

| Approach | Space | Rank Time | Select Time | Build Time |
|----------|-------|-----------|-------------|------------|
| Naive (std::vector<bool>) | n bits | O(n) | O(n) | O(1) |
| Dense rank array | 32n bits | O(1) | O(1) | O(n) |
| SuccinctBitVector | n + 0.2%n | O(1) | O(log n) | O(n) |

## Integration with PFC

SuccinctBitVector implements the PackedValue concept and integrates seamlessly:

```cpp
// Use in PackedVector
PackedVector<SuccinctBitVector> bitvector_collection;
bitvector_collection.push_back(bv1);
bitvector_collection.push_back(bv2);

// Use in PackedOptional
PackedOptional<SuccinctBitVector> maybe_bv = bv;

// Compose with other codecs
using CompactBitVec = Packed<SuccinctBitVector, codecs::EliasGamma>;
```

## Future Succinct Structures

The SuccinctBitVector serves as the foundation for additional succinct structures planned:

- **RoaringBitmap**: Hybrid compressed integer sets
- **WaveletTree**: Sequence rank/select on arbitrary alphabets
- **InterpolativeCoding**: Optimal compression for sorted sequences
- **PackedGraph**: Succinct graph representations
- **Rank-Select Dictionary**: For very sparse bit vectors

See [SUCCINCT_DESIGN.md](https://github.com/queelius/pfc/blob/main/SUCCINCT_DESIGN.md) for detailed design documentation.

## References

1. Jacobson, G. (1989). "Space-efficient static trees and graphs"
2. Clark, D. (1996). "Compact Pat Trees"
3. Vigna, S. (2008). "Broadword Implementation of Rank/Select Queries"

## See Also

- [Core API](core.md) - BitReader/BitWriter fundamentals
- [Codecs](codecs.md) - Universal integer codes
- [Packed Types](packed.md) - PackedValue concept
