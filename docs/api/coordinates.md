# Coordinates API

Geometric types with packed storage.

```cpp
#include <pfc/coordinates.hpp>
```

## Vectors

```cpp
template<typename Codec>
class PackedVector2 {
public:
    PackedVector2(double x, double y);
    double x() const;
    double y() const;
    double length() const;
    double dot(const PackedVector2& other) const;
};

template<typename Codec>
class PackedVector3;  // Similar interface

template<typename Codec>
class PackedVector4;
```

## Matrices

```cpp
template<size_t Rows, size_t Cols, typename Codec>
class PackedMatrix {
public:
    double operator()(size_t i, size_t j) const;
    PackedMatrix operator*(const PackedMatrix& other) const;
};
```

## Transformations

```cpp
template<typename Codec>
class AffineTransform {
public:
    static AffineTransform translation(double x, double y, double z);
    static AffineTransform rotation(double angle, const Vector3& axis);
    static AffineTransform scale(double sx, double sy, double sz);

    Vector3 apply(const Vector3& point) const;
};
```

See [API Overview](overview.md) for complete documentation.
