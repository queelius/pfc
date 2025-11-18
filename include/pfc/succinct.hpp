#pragma once
// succinct.hpp - Succinct data structures with zero-copy semantics
// Wire format = in-memory format: The ultimate competitive advantage

#include "core.hpp"
#include "codecs.hpp"
#include "packed.hpp"
#include <vector>
#include <span>
#include <cmath>

namespace pfc::succinct {

// ============================================================
//  Succinct Bit Vector - Foundation for all succinct structures
//  Space: n + o(n) bits with O(1) rank/select operations
// ============================================================

// Forward declarations
class SuccinctBitVector;

// ============================================================
//  Rank/Select Support - The core operations
// ============================================================

// Concept for rank/select query structures
template<typename T>
concept RankSelectSupport = requires(const T& rs, size_t pos) {
    { rs.rank(pos) } -> std::convertible_to<size_t>;  // Count 1s up to pos
    { rs.select(pos) } -> std::convertible_to<size_t>; // Find pos-th 1
};

// ============================================================
//  Block-based Rank Support
//  Uses superblocks and blocks for O(1) rank queries
// ============================================================

class BlockRankSupport {
private:
    // Superblock size: every 2^16 bits (8KB)
    static constexpr size_t SUPERBLOCK_SIZE = 65536;
    static constexpr size_t SUPERBLOCK_SHIFT = 16;

    // Block size: every 512 bits (64 bytes, one cache line)
    static constexpr size_t BLOCK_SIZE = 512;
    static constexpr size_t BLOCK_SHIFT = 9;
    static constexpr size_t BLOCKS_PER_SUPERBLOCK = SUPERBLOCK_SIZE / BLOCK_SIZE;

    std::vector<uint32_t> superblocks_;  // Absolute rank at superblock boundaries
    std::vector<uint16_t> blocks_;       // Relative rank within superblock
    const uint64_t* bits_;               // Pointer to bit vector
    size_t num_bits_;

public:
    BlockRankSupport() = default;

    // Build from bit vector
    void build(const uint64_t* bits, size_t num_bits) {
        bits_ = bits;
        num_bits_ = num_bits;

        size_t num_superblocks = (num_bits + SUPERBLOCK_SIZE - 1) / SUPERBLOCK_SIZE;
        size_t num_blocks = (num_bits + BLOCK_SIZE - 1) / BLOCK_SIZE;

        superblocks_.resize(num_superblocks + 1);
        blocks_.resize(num_blocks + 1);

        size_t total_rank = 0;
        size_t superblock_rank = 0;

        for (size_t i = 0; i < num_blocks; ++i) {
            size_t bit_start = i * BLOCK_SIZE;
            size_t superblock_idx = bit_start / SUPERBLOCK_SIZE;

            // Start of new superblock
            if (bit_start % SUPERBLOCK_SIZE == 0) {
                superblocks_[superblock_idx] = total_rank;
                superblock_rank = 0;
            }

            // Store relative rank within superblock
            blocks_[i] = superblock_rank;

            // Count bits in this block
            size_t block_rank = 0;
            size_t word_start = bit_start / 64;
            size_t word_end = std::min((bit_start + BLOCK_SIZE) / 64, (num_bits + 63) / 64);

            for (size_t w = word_start; w < word_end; ++w) {
                block_rank += __builtin_popcountll(bits_[w]);
            }

            total_rank += block_rank;
            superblock_rank += block_rank;
        }

        superblocks_[num_superblocks] = total_rank;
    }

    // Count 1s in range [0, pos)
    [[nodiscard]] size_t rank(size_t pos) const {
        if (pos == 0) return 0;
        if (pos >= num_bits_) {
            // Return total rank when at or past end
            size_t num_superblocks = (num_bits_ + SUPERBLOCK_SIZE - 1) / SUPERBLOCK_SIZE;
            return superblocks_[num_superblocks];
        }

        size_t superblock_idx = pos / SUPERBLOCK_SIZE;
        size_t block_idx = pos / BLOCK_SIZE;
        size_t word_idx = pos / 64;
        size_t bit_offset = pos % 64;

        size_t result = superblocks_[superblock_idx];
        result += blocks_[block_idx];

        // Count remaining bits in partial word
        size_t block_start = (pos / BLOCK_SIZE) * BLOCK_SIZE;
        size_t word_start = block_start / 64;

        for (size_t w = word_start; w < word_idx; ++w) {
            result += __builtin_popcountll(bits_[w]);
        }

        if (bit_offset > 0) {
            uint64_t mask = (1ULL << bit_offset) - 1;
            result += __builtin_popcountll(bits_[word_idx] & mask);
        }

        return result;
    }

    // Find position of k-th 1 (0-indexed)
    [[nodiscard]] size_t select(size_t k) const {
        // Binary search on superblocks
        size_t left = 0, right = superblocks_.size() - 1;

        while (left < right) {
            size_t mid = left + (right - left) / 2;
            if (superblocks_[mid] <= k) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }

        size_t superblock_idx = left > 0 ? left - 1 : 0;
        size_t remaining = k - superblocks_[superblock_idx];

        // Linear search within superblock (could be optimized)
        size_t block_start = superblock_idx * (SUPERBLOCK_SIZE / BLOCK_SIZE);
        size_t block_end = std::min(block_start + BLOCKS_PER_SUPERBLOCK, blocks_.size());

        for (size_t b = block_start; b < block_end; ++b) {
            size_t bit_start = b * BLOCK_SIZE;
            size_t word_start = bit_start / 64;
            size_t word_end = std::min((bit_start + BLOCK_SIZE) / 64, (num_bits_ + 63) / 64);

            for (size_t w = word_start; w < word_end; ++w) {
                size_t popcount = __builtin_popcountll(bits_[w]);
                if (remaining < popcount) {
                    // Found the word containing k-th 1
                    uint64_t word = bits_[w];
                    for (size_t i = 0; i < 64; ++i) {
                        if (word & (1ULL << i)) {
                            if (remaining == 0) {
                                return w * 64 + i;
                            }
                            --remaining;
                        }
                    }
                }
                remaining -= popcount;
            }
        }

        return num_bits_; // Not found
    }

    // Serialization
    template<typename Sink>
    void encode(Sink& sink) const requires BitSink<Sink> {
        // Encode structure metadata
        codecs::EliasGamma::encode(static_cast<uint32_t>(num_bits_), sink);
        codecs::EliasGamma::encode(static_cast<uint32_t>(superblocks_.size()), sink);

        for (auto sb : superblocks_) {
            codecs::EliasGamma::encode(sb, sink);
        }

        for (auto b : blocks_) {
            codecs::Fixed<16>::encode(b, sink);
        }
    }

    template<typename Source>
    static BlockRankSupport decode(Source& source, const uint64_t* bits)
        requires BitSource<Source>
    {
        BlockRankSupport rs;
        rs.bits_ = bits;
        rs.num_bits_ = codecs::EliasGamma::decode<uint32_t>(source);

        size_t num_superblocks = codecs::EliasGamma::decode<uint32_t>(source);
        rs.superblocks_.resize(num_superblocks);

        for (auto& sb : rs.superblocks_) {
            sb = codecs::EliasGamma::decode<uint32_t>(source);
        }

        size_t num_blocks = (rs.num_bits_ + BLOCK_SIZE - 1) / BLOCK_SIZE;
        rs.blocks_.resize(num_blocks);

        for (auto& b : rs.blocks_) {
            b = codecs::Fixed<16>::decode<uint16_t>(source);
        }

        return rs;
    }
};

// ============================================================
//  Succinct Bit Vector - The complete package
// ============================================================

class SuccinctBitVector {
private:
    std::vector<uint64_t> bits_;
    size_t num_bits_ = 0;
    BlockRankSupport rank_support_;

public:
    using value_type = bool;
    using size_type = size_t;

    SuccinctBitVector() = default;

    explicit SuccinctBitVector(size_t num_bits, bool initial_value = false)
        : bits_((num_bits + 63) / 64, initial_value ? ~0ULL : 0ULL)
        , num_bits_(num_bits)
    {
        // Clear unused bits in last word
        if (num_bits_ % 64 != 0) {
            size_t last_word = bits_.size() - 1;
            size_t valid_bits = num_bits_ % 64;
            bits_[last_word] &= (1ULL << valid_bits) - 1;
        }
    }

    // Construct from bit pattern
    SuccinctBitVector(std::initializer_list<bool> bits)
        : bits_((bits.size() + 63) / 64, 0)
        , num_bits_(bits.size())
    {
        size_t i = 0;
        for (bool b : bits) {
            if (b) set(i);
            ++i;
        }
        build_rank_support();
    }

    // Build rank/select support (call after modifications)
    void build_rank_support() {
        rank_support_.build(bits_.data(), num_bits_);
    }

    // Size queries
    [[nodiscard]] size_t size() const noexcept { return num_bits_; }
    [[nodiscard]] size_t num_words() const noexcept { return bits_.size(); }
    [[nodiscard]] bool empty() const noexcept { return num_bits_ == 0; }

    // Bit access (read-only for zero-copy semantics)
    [[nodiscard]] bool operator[](size_t pos) const {
        size_t word_idx = pos / 64;
        size_t bit_idx = pos % 64;
        return (bits_[word_idx] >> bit_idx) & 1;
    }

    [[nodiscard]] bool test(size_t pos) const {
        return (*this)[pos];
    }

    // Bit modification (invalidates rank support)
    void set(size_t pos, bool value = true) {
        size_t word_idx = pos / 64;
        size_t bit_idx = pos % 64;
        if (value) {
            bits_[word_idx] |= (1ULL << bit_idx);
        } else {
            bits_[word_idx] &= ~(1ULL << bit_idx);
        }
    }

    void reset(size_t pos) {
        set(pos, false);
    }

    void flip(size_t pos) {
        size_t word_idx = pos / 64;
        size_t bit_idx = pos % 64;
        bits_[word_idx] ^= (1ULL << bit_idx);
    }

    // Rank: count 1s in [0, pos)
    [[nodiscard]] size_t rank(size_t pos) const {
        return rank_support_.rank(pos);
    }

    // Select: find position of k-th 1 (0-indexed)
    [[nodiscard]] size_t select(size_t k) const {
        return rank_support_.select(k);
    }

    // Access raw bit data
    [[nodiscard]] const uint64_t* data() const noexcept {
        return bits_.data();
    }

    [[nodiscard]] std::span<const uint64_t> words() const noexcept {
        return bits_;
    }

    // Serialization - ZERO COPY WIRE FORMAT
    template<typename Sink>
    static void encode(const SuccinctBitVector& bv, Sink& sink)
        requires BitSink<Sink>
    {
        // Encode number of bits
        codecs::EliasGamma::encode(static_cast<uint32_t>(bv.num_bits_), sink);

        // Encode bit data directly (zero-copy!)
        for (auto word : bv.bits_) {
            for (int i = 0; i < 64; ++i) {
                sink.write((word >> i) & 1);
            }
        }

        // Encode rank support structure
        bv.rank_support_.encode(sink);
    }

    template<typename Source>
    static SuccinctBitVector decode(Source& source)
        requires BitSource<Source>
    {
        SuccinctBitVector bv;

        // Decode number of bits
        bv.num_bits_ = codecs::EliasGamma::decode<uint32_t>(source);

        // Decode bit data
        size_t num_words = (bv.num_bits_ + 63) / 64;
        bv.bits_.resize(num_words);

        for (size_t w = 0; w < num_words; ++w) {
            uint64_t word = 0;
            for (int i = 0; i < 64; ++i) {
                word |= static_cast<uint64_t>(source.read()) << i;
            }
            bv.bits_[w] = word;
        }

        // Decode rank support
        bv.rank_support_ = BlockRankSupport::decode(source, bv.bits_.data());

        return bv;
    }
};

} // namespace pfc::succinct

// Make SuccinctBitVector a PackedValue (must be in std namespace)
namespace std {
template<>
struct tuple_size<pfc::succinct::SuccinctBitVector>
    : std::integral_constant<size_t, 1> {};
}

// ============================================================
//  DESIGN NOTES FOR FUTURE SUCCINCT STRUCTURES
// ============================================================

/*

ROARING BITMAPS
===============
Compressed integer sets with O(1) membership, efficient set operations.

Design sketch:

class RoaringBitmap {
    // Three container types based on cardinality:
    // - Array: sorted uint16_t array (< 4096 elements)
    // - Bitmap: 8KB bit vector (dense sets)
    // - Run: run-length encoded runs

    struct Chunk {
        uint16_t key;        // High 16 bits
        uint8_t type;        // 0=array, 1=bitmap, 2=run
        PackedVector<uint16_t> data;  // Low 16 bits
    };

    PackedVector<Chunk> chunks_;

    // STL set-like interface
    bool contains(uint32_t x) const;
    void insert(uint32_t x);
    void remove(uint32_t x);

    // Set operations (zero-copy where possible)
    RoaringBitmap union_with(const RoaringBitmap& other) const;
    RoaringBitmap intersect_with(const RoaringBitmap& other) const;

    // Zero-copy wire format
    template<typename Sink>
    static void encode(const RoaringBitmap& rb, Sink& sink);
};

INTERPOLATIVE CODING
====================
Optimal compression for sorted integer sequences.

class InterpolativeCoded {
    SuccinctBitVector bits_;
    size_t universe_size_;
    size_t num_elements_;

    // Recursive encoding of sorted integers
    void encode_range(const std::vector<uint32_t>& vals,
                     size_t low, size_t high,
                     uint32_t min_val, uint32_t max_val);

    // O(log n) access with binary interpolation search
    uint32_t operator[](size_t index) const;

    // Zero-copy wire format
    template<typename Sink>
    static void encode(const InterpolativeCoded& ic, Sink& sink);
};

WAVELET TREE
============
Powerful sequence structure with rank/select on arbitrary alphabets.

template<typename Symbol = uint8_t>
class WaveletTree {
    SuccinctBitVector level_bits_;
    std::vector<size_t> level_boundaries_;
    size_t alphabet_size_;

    // O(log σ) rank/select for any symbol
    size_t rank(Symbol s, size_t pos) const;
    size_t select(Symbol s, size_t k) const;

    // O(log σ) access
    Symbol operator[](size_t pos) const;

    // Range queries
    std::vector<Symbol> range(size_t start, size_t end) const;

    // Zero-copy wire format
    template<typename Sink>
    static void encode(const WaveletTree& wt, Sink& sink);
};

PACKED GRAPHS
=============
Succinct graph representations with zero-copy semantics.

// Compressed Sparse Row (CSR) format
class PackedGraph {
    PackedVector<uint32_t, codecs::EliasDelta> offsets_;
    PackedVector<uint32_t, codecs::VByte> edges_;

    struct Neighbors {
        // Proxy iterator for zero-copy access
        class iterator {
            const PackedGraph* graph_;
            size_t pos_;

            uint32_t operator*() const {
                return graph_->edges_[pos_];
            }
        };

        iterator begin() const;
        iterator end() const;
    };

    // Graph queries
    size_t num_vertices() const;
    size_t num_edges() const;
    Neighbors neighbors(uint32_t v) const;

    // Zero-copy wire format
    template<typename Sink>
    static void encode(const PackedGraph& g, Sink& sink);
};

RANK-SELECT DICTIONARY
======================
Space-efficient representation of sparse bit vectors.

class RankSelectDictionary {
    // Store only positions of 1s
    PackedVector<uint32_t, codecs::EliasDelta> ones_;
    size_t universe_size_;

    // O(log n) rank via binary search
    size_t rank(size_t pos) const;

    // O(1) select via direct access
    size_t select(size_t k) const { return ones_[k]; }

    // Zero-copy wire format
    template<typename Sink>
    static void encode(const RankSelectDictionary& rsd, Sink& sink);
};

DESIGN PRINCIPLES
=================

1. ZERO-COPY FIRST
   - Wire format = in-memory format
   - No parsing step, direct memory mapping
   - Prefix-free encoding for self-delimiting

2. COMPOSABILITY
   - Build on existing PackedValue types
   - Use codec composition (VByte for metadata, Delta for deltas)
   - Mix and match structures

3. STL-ADJACENT
   - Familiar interfaces (vector-like, set-like)
   - Standard algorithms work via proxy iterators
   - Range support

4. PERFORMANCE
   - Cache-friendly layouts (64-byte blocks)
   - SIMD-ready where applicable (VByte, popcount)
   - O(1) or O(log n) operations

5. IMMUTABLE BY DEFAULT
   - Builders for construction
   - Frozen structures for queries
   - Clear separation of concerns

EXAMPLE USAGE
=============

// Build a succinct bit vector
SuccinctBitVector bv(1000);
for (size_t i = 0; i < 1000; i += 7) {
    bv.set(i);
}
bv.build_rank_support();

// Query operations
size_t ones_before_100 = bv.rank(100);  // O(1)
size_t tenth_one = bv.select(10);       // O(log n)

// Zero-copy serialization
uint8_t buffer[10000];
BitWriter writer(buffer);
SuccinctBitVector::encode(bv, writer);
writer.align();

// Zero-copy deserialization (no parsing!)
BitReader reader(buffer, 10000);
auto bv2 = SuccinctBitVector::decode(reader);

// Use in packed structures
using PackedBitVec = Packed<SuccinctBitVector, SuccinctBitVector>;
PackedVector<PackedBitVec> collection;

// Roaring bitmap example (future)
RoaringBitmap bitmap;
for (uint32_t i : {1, 5, 100, 1000, 100000}) {
    bitmap.insert(i);
}

auto bitmap2 = bitmap.union_with(other_bitmap);  // Zero-copy where possible

// Wavelet tree example (future)
WaveletTree<char> wt("the quick brown fox");
size_t q_count = wt.rank('q', wt.size());  // Count 'q's
size_t first_q = wt.select('q', 0);        // Find first 'q'

*/
