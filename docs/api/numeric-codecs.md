# Numeric Codecs API

Codecs for floating point, rational, and complex numbers.

## FloatingPoint

Configurable precision floating point encoding.

```cpp
template<unsigned MantissaBits, unsigned ExponentBits>
struct FloatingPoint;

// Standard formats
using Float16 = FloatingPoint<11, 5>;   // IEEE 754 half
using Float32 = FloatingPoint<24, 8>;   // IEEE 754 single
using Float64 = FloatingPoint<53, 11>;  // IEEE 754 double
using BFloat16 = FloatingPoint<8, 8>;   // Google Brain float
```

## Rational

Exact fraction representation.

```cpp
class PackedRational {
public:
    PackedRational(int numerator, int denominator);

    int numerator() const;
    int denominator() const;
    double to_double() const;
};
```

## Complex

Complex number encoding (rectangular or polar).

```cpp
template<typename FloatCodec>
class ComplexRectangular;

template<typename FloatCodec>
class ComplexPolar;
```

See [API Overview](overview.md) for complete documentation.
