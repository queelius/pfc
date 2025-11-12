# Packed Types API

Packed types maintain the zero-copy invariant by storing values in compressed form.

## Packed

Single packed value with specified codec.

```cpp
template<typename T, typename Codec>
class Packed {
public:
    Packed();
    explicit Packed(const T& value);

    T get() const;
    operator T() const;

    size_t byte_size() const;
    const uint8_t* data() const;

    bool operator==(const Packed& other) const;
    bool operator!=(const Packed& other) const;
    bool operator<(const Packed& other) const;
};
```

## PackedVector

Dynamic array with zero-copy semantics.

```cpp
template<typename T>
class PackedVector {
public:
    using value_type = T;
    using iterator = /* proxy iterator */;

    PackedVector();
    explicit PackedVector(size_t count);

    void push_back(const T& value);
    void pop_back();
    T operator[](size_t index) const;
    T at(size_t index) const;

    size_t size() const;
    bool empty() const;
    void clear();

    iterator begin();
    iterator end();

    size_t byte_size() const;
    const uint8_t* data() const;
};
```

See [API Overview](overview.md) for complete documentation.
