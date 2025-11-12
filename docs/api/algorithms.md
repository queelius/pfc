# Algorithms API

Generic algorithms optimized for packed data.

```cpp
#include <pfc/algorithms.hpp>
```

## Transform

```cpp
template<typename InputIt, typename OutputIt, typename F>
OutputIt packed_transform(InputIt first, InputIt last,
                         OutputIt d_first, F&& func);
```

## Filter

```cpp
template<typename InputIt, typename OutputIt, typename Pred>
OutputIt packed_filter(InputIt first, InputIt last,
                      OutputIt d_first, Pred&& pred);
```

## Reduce

```cpp
template<typename InputIt, typename T, typename BinaryOp>
T packed_reduce(InputIt first, InputIt last,
               T init, BinaryOp&& op);
```

## Parallel Execution

With TBB support:

```cpp
#include <execution>

auto result = packed_transform(
    std::execution::par_unseq,
    data.begin(), data.end(),
    output.begin(),
    [](auto x) { return expensive_compute(x); }
);
```

See [API Overview](overview.md) for complete documentation.
