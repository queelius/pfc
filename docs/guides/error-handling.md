# Error Handling Guide

Best practices for robust error handling with PFC.

## Use Result Types

Prefer Result over exceptions for expected errors:

```cpp
using Result = PackedResult<Packed<int, EliasGamma>, PackedString>;

Result parse_integer(const std::string& s) {
    try {
        return Result::ok(std::stoi(s));
    } catch (...) {
        return Result::err("Invalid integer format");
    }
}

// Caller handles error explicitly
auto result = parse_integer(input);
if (result.is_ok()) {
    process(result.unwrap());
} else {
    log_error(result.unwrap_err());
}
```

## Chain Operations

Use monadic operations for error propagation:

```cpp
auto result = parse_integer("42")
    .map([](int x) { return x * 2; })
    .and_then([](int x) { return validate_range(x, 0, 100); })
    .map([](int x) { return format_output(x); });
```

## Pattern Matching

Use visit for exhaustive handling:

```cpp
result.visit(overloaded{
    [](const auto& value) {
        std::cout << "Success: " << value << "\n";
    },
    [](const std::string& error) {
        std::cerr << "Error: " << error << "\n";
    }
});
```

## Validation

Validate inputs early:

```cpp
Result create_user(const std::string& name, int age) {
    if (name.empty()) {
        return Result::err("Name cannot be empty");
    }
    if (age < 0 || age > 150) {
        return Result::err("Invalid age");
    }
    return Result::ok(User{name, age});
}
```

See [Error Handling API](../api/error-handling.md) and [Algebraic Types](../api/algebraic.md).
