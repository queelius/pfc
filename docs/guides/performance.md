# Performance Optimization

Guidelines for getting the best performance from PFC.

## Choose the Right Codec

Different codecs have different performance characteristics:

- **Unary**: Fastest encoding/decoding, worst compression
- **Fixed-width**: Predictable, no branching
- **Elias codes**: Good balance of speed and compression
- **Huffman/LZ77**: Best compression, slower

## Batch Operations

Process multiple values together:

```cpp
// Slower: Individual operations
for (auto value : values) {
    packed_vec.push_back(value);
}

// Faster: Batch insert
packed_vec.reserve(values.size());
std::copy(values.begin(), values.end(),
          std::back_inserter(packed_vec));
```

## Use Parallel Algorithms

With TBB:

```cpp
#include <execution>

packed_transform(std::execution::par_unseq,
                data.begin(), data.end(),
                output.begin(),
                expensive_function);
```

## Profile First

Use profiling tools to find bottlenecks:

```bash
perf record ./your_app
perf report
```

Common hotspots:
- Bit I/O operations
- Decode-on-access in tight loops
- Unnecessary copies

## Memory Layout

Consider alignment and cache behavior:

```cpp
// Good: Sequential access
for (size_t i = 0; i < vec.size(); ++i) {
    process(vec[i]);
}

// Bad: Random access in packed data
for (size_t i : random_indices) {
    process(vec[i]);  // O(n) worst case
}
```

See [Design Principles](design-principles.md) for performance characteristics.
