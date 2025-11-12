#pragma once
// codecs.hpp - Elegant prefix-free codec implementations
// Each codec is a work of art, simple yet powerful

#include "core.hpp"
#include <limits>
#include <array>

namespace pfc::codecs {

// ============================================================
//  Unary Codec - The simplest possible variable-length code
// ============================================================

struct Unary {
    template<std::unsigned_integral T, typename Sink>
    static void encode(T value, Sink& sink) requires BitSink<Sink> {
        // Write n zeros followed by a one
        for (T i = 0; i < value; ++i) {
            sink.write(false);
        }
        sink.write(true);
    }

    template<std::unsigned_integral T, typename Source>
    static T decode(Source& source) requires BitSource<Source> {
        T count = 0;
        while (!source.read()) {
            ++count;
        }
        return count;
    }
};

// ============================================================
//  Elias Gamma - The fundamental universal code
// ============================================================

struct EliasGamma {
    template<std::unsigned_integral T, typename Sink>
    static void encode(T value, Sink& sink) requires BitSink<Sink> {
        ++value; // Map 0→1, 1→2, etc. for gamma coding
        
        int len = msb_position(value) + 1;
        
        // Write len-1 zeros
        for (int i = 0; i < len - 1; ++i) {
            sink.write(false);
        }
        
        // Write value in binary (MSB to LSB)
        for (int i = len - 1; i >= 0; --i) {
            sink.write((value >> i) & 1);
        }
    }

    template<std::unsigned_integral T>
    static T decode(auto& source) requires BitSource<decltype(source)> {
        // Count leading zeros
        int zeros = 0;
        while (!source.read()) {
            ++zeros;
        }
        
        // Read zeros more bits
        T value = 1;
        for (int i = 0; i < zeros; ++i) {
            value = (value << 1) | source.read();
        }
        
        return value - 1; // Map back: 1→0, 2→1, etc.
    }
};

// ============================================================
//  Elias Delta - More efficient for larger numbers
// ============================================================

struct EliasDelta {
    template<std::unsigned_integral T>
    static void encode(T value, auto& sink) requires BitSink<decltype(sink)> {
        ++value; // Map 0→1
        
        int len = msb_position(value) + 1;
        
        // Encode len using gamma
        EliasGamma::encode(static_cast<T>(len - 1), sink);
        
        // Write remaining bits of value (skip MSB which is always 1)
        for (int i = len - 2; i >= 0; --i) {
            sink.write((value >> i) & 1);
        }
    }

    template<std::unsigned_integral T>
    static T decode(auto& source) requires BitSource<decltype(source)> {
        // Decode length
        uint32_t len = EliasGamma::decode<uint32_t>(source) + 1;
        
        // Read remaining bits
        T value = 1;
        for (uint32_t i = 0; i < len - 1; ++i) {
            value = (value << 1) | source.read();
        }
        
        return value - 1;
    }
};

// ============================================================
//  Fibonacci Codec - Based on Fibonacci sequence
// ============================================================

struct Fibonacci {
private:
    static constexpr size_t MAX_FIBS = 92; // Enough for 64-bit
    
    template<typename T>
    static constexpr std::array<T, MAX_FIBS> compute_fibs() {
        std::array<T, MAX_FIBS> fibs{};
        fibs[0] = 1;
        fibs[1] = 2;
        for (size_t i = 2; i < MAX_FIBS && fibs[i-1] < std::numeric_limits<T>::max() / 2; ++i) {
            fibs[i] = fibs[i-1] + fibs[i-2];
        }
        return fibs;
    }

public:
    template<std::unsigned_integral T>
    static void encode(T value, auto& sink) requires BitSink<decltype(sink)> {
        if (value == 0) {
            sink.write(true);
            sink.write(true);
            return;
        }
        
        constexpr auto fibs = compute_fibs<T>();
        ++value; // Map to positive integers
        
        // Find Fibonacci representation
        std::array<bool, MAX_FIBS> bits{};
        int highest = 0;
        
        for (int i = MAX_FIBS - 1; i >= 0; --i) {
            if (fibs[i] <= value && fibs[i] > 0) {
                bits[i] = true;
                value -= fibs[i];
                highest = std::max(highest, i);
            }
        }
        
        // Write bits from lowest to highest, then terminator
        for (int i = 0; i <= highest; ++i) {
            sink.write(bits[i]);
        }
        sink.write(true); // Terminator
    }

    template<std::unsigned_integral T>
    static T decode(auto& source) requires BitSource<decltype(source)> {
        constexpr auto fibs = compute_fibs<T>();
        
        T value = 0;
        bool prev_bit = false;
        
        for (size_t i = 0; i < MAX_FIBS; ++i) {
            bool bit = source.read();
            
            if (bit && prev_bit) {
                // Found terminator (two consecutive 1s)
                return value > 0 ? value - 1 : 0;
            }
            
            if (bit) {
                value += fibs[i];
            }
            
            prev_bit = bit;
        }
        
        return value > 0 ? value - 1 : 0;
    }
};

// ============================================================
//  Rice Codec - Optimal for geometric distributions
// ============================================================

template<size_t K>
struct Rice {
    static_assert(K < 64, "K must be less than 64");
    
    template<std::unsigned_integral T>
    static void encode(T value, auto& sink) requires BitSink<decltype(sink)> {
        T q = value >> K;  // Quotient
        T r = value & ((1u << K) - 1);  // Remainder
        
        // Encode quotient in unary
        Unary::encode(q, sink);
        
        // Encode remainder in binary
        for (size_t i = 0; i < K; ++i) {
            sink.write((r >> i) & 1);
        }
    }

    template<std::unsigned_integral T>
    static T decode(auto& source) requires BitSource<decltype(source)> {
        // Decode quotient
        T q = Unary::decode<T>(source);
        
        // Decode remainder
        T r = 0;
        for (size_t i = 0; i < K; ++i) {
            r |= static_cast<T>(source.read()) << i;
        }
        
        return (q << K) | r;
    }
};

// ============================================================
//  Fixed Width - Sometimes simplicity wins
// ============================================================

template<size_t Width>
struct Fixed {
    static_assert(Width > 0 && Width <= 64, "Width must be between 1 and 64");
    
    template<std::unsigned_integral T>
    static void encode(T value, auto& sink) requires BitSink<decltype(sink)> {
        for (size_t i = 0; i < Width; ++i) {
            sink.write((value >> i) & 1);
        }
    }

    template<std::unsigned_integral T>
    static T decode(auto& source) requires BitSource<decltype(source)> {
        T value = 0;
        for (size_t i = 0; i < Width; ++i) {
            value |= static_cast<T>(source.read()) << i;
        }
        return value;
    }
};

// ============================================================
//  Adaptive Codec - Choose codec based on value distribution
// ============================================================

template<typename SmallCodec, typename LargeCodec, std::unsigned_integral T = uint32_t>
struct Adaptive {
    static constexpr T threshold = 128;  // Tunable parameter
    
    static void encode(T value, auto& sink) requires BitSink<decltype(sink)> {
        if (value < threshold) {
            sink.write(false);  // Flag for small codec
            SmallCodec::encode(value, sink);
        } else {
            sink.write(true);   // Flag for large codec
            LargeCodec::encode(value - threshold, sink);
        }
    }

    static T decode(auto& source) requires BitSource<decltype(source)> {
        if (!source.read()) {
            return SmallCodec::template decode<T>(source);
        } else {
            return LargeCodec::template decode<T>(source) + threshold;
        }
    }
};

// ============================================================
//  Boolean Codec - The atom of information
// ============================================================

struct Boolean {
    static void encode(bool value, auto& sink) requires BitSink<decltype(sink)> {
        sink.write(value);
    }

    static bool decode(auto& source) requires BitSource<decltype(source)> {
        return source.read();
    }
};

// ============================================================
//  Signed Integer Codec - Zigzag encoding
// ============================================================

template<typename UnsignedCodec>
struct Signed {
    template<std::signed_integral T>
    static void encode(T value, auto& sink) requires BitSink<decltype(sink)> {
        using U = std::make_unsigned_t<T>;
        // Zigzag encoding: 0→0, -1→1, 1→2, -2→3, 2→4...
        U encoded = (value < 0) ? ((-value) * 2 - 1) : (value * 2);
        UnsignedCodec::encode(encoded, sink);
    }

    template<std::signed_integral T>
    static T decode(auto& source) requires BitSource<decltype(source)> {
        using U = std::make_unsigned_t<T>;
        U encoded = UnsignedCodec::template decode<U>(source);
        // Zigzag decoding
        return (encoded & 1) ? -static_cast<T>((encoded + 1) / 2) 
                              : static_cast<T>(encoded / 2);
    }
};

// Convenience aliases
using SignedGamma = Signed<EliasGamma>;
using SignedDelta = Signed<EliasDelta>;
using SignedFibonacci = Signed<Fibonacci>;

} // namespace pfc::codecs