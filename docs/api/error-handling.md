# Error Handling API

Robust error handling with Result types.

```cpp
#include <pfc/error_handling.hpp>
```

## Result Type

```cpp
template<typename T, typename E>
class PackedResult {
public:
    static PackedResult ok(const T& value);
    static PackedResult err(const E& error);

    bool is_ok() const;
    bool is_err() const;

    T unwrap();  // Throws if error
    E unwrap_err();  // Throws if ok
    T unwrap_or(T default_value) const;

    // Monadic operations
    template<typename F>
    auto map(F&& f);

    template<typename F>
    auto and_then(F&& f);
};
```

## Usage Example

```cpp
using Result = PackedResult<Packed<int, EliasGamma>, PackedString>;

Result divide(int a, int b) {
    if (b == 0) {
        return Result::err("Division by zero");
    }
    return Result::ok(a / b);
}

auto result = divide(10, 2);
if (result.is_ok()) {
    std::cout << "Result: " << result.unwrap() << "\n";
} else {
    std::cerr << "Error: " << result.unwrap_err() << "\n";
}

// Chaining
auto final_result = divide(10, 2)
    .map([](int x) { return x * 2; })
    .and_then([](int x) { return divide(x, 5); });
```

See [Error Handling Guide](../guides/error-handling.md) and [Algebraic Types](algebraic.md).
