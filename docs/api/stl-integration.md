# STL Integration API

PFC containers and iterators work seamlessly with STL algorithms.

## PackedContainer

STL-compatible container with packed storage.

```cpp
template<typename T, typename Codec>
class PackedContainer {
public:
    using value_type = T;
    using iterator = /* random access iterator */;
    using const_iterator = /* const random access iterator */;

    // STL interface
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    size_t size() const;
    bool empty() const;
    void clear();

    void push_back(const T& value);
    void pop_back();
    T operator[](size_t index) const;
};
```

## STL Algorithm Compatibility

All standard algorithms work:

```cpp
PackedContainer<uint32_t, EliasGamma> data;

// Sorting
std::sort(data.begin(), data.end());
std::stable_sort(data.begin(), data.end());

// Searching
auto it = std::find(data.begin(), data.end(), 42);
auto it2 = std::binary_search(data.begin(), data.end(), 42);

// Accumulation
auto sum = std::accumulate(data.begin(), data.end(), 0);

// Transformation
std::transform(data.begin(), data.end(), data.begin(),
               [](auto x) { return x * 2; });
```

See [API Overview](overview.md) for complete documentation.
