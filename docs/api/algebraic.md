# Algebraic Types API

Algebraic types enable powerful type composition through sum types (OR) and product types (AND).

## Header

```cpp
#include <pfc/algebraic.hpp>
```

## PackedOptional (Maybe)

Represents an optional value that may or may not be present.

### Interface

```cpp
template<typename T>
class PackedOptional {
public:
    // Constructors
    PackedOptional();                          // None
    PackedOptional(std::nullopt_t);           // None
    PackedOptional(const T& value);           // Some
    PackedOptional(T&& value);                // Some

    // Queries
    bool has_value() const;
    explicit operator bool() const;

    // Access
    T& value();
    const T& value() const;
    T& operator*();
    const T& operator*() const;
    T* operator->();
    const T* operator->() const;

    // Fallback
    T value_or(T default_value) const;

    // Modification
    void reset();
    template<typename... Args>
    T& emplace(Args&&... args);

    // Zero-copy interface
    size_t byte_size() const;
    const uint8_t* data() const;
};
```

### Usage

```cpp
using OptionalInt = PackedOptional<Packed<int, Signed<EliasGamma>>>;

// Create values
OptionalInt some(42);
OptionalInt none(std::nullopt);

// Check and access
if (some) {
    std::cout << "Value: " << *some << "\n";
}

// Fallback
int value = none.value_or(0);

// Pattern matching
some.visit(overloaded{
    [](const auto& v) { std::cout << "Some: " << v << "\n"; },
    []() { std::cout << "None\n"; }
});
```

## PackedEither

Represents one of two possible types (tagged union).

### Interface

```cpp
template<typename L, typename R>
class PackedEither {
public:
    // Constructors
    static PackedEither left(const L& value);
    static PackedEither right(const R& value);

    // Queries
    bool is_left() const;
    bool is_right() const;

    // Access (throws if wrong type)
    L& left();
    const L& left() const;
    R& right();
    const R& right() const;

    // Visitors
    template<typename F>
    auto visit(F&& visitor);

    // Zero-copy interface
    size_t byte_size() const;
    const uint8_t* data() const;
};
```

### Usage

```cpp
using IntOrString = PackedEither<
    Packed<int, Signed<EliasGamma>>,
    PackedString
>;

// Create values
auto int_val = IntOrString::left(42);
auto str_val = IntOrString::right("hello");

// Check type
if (int_val.is_left()) {
    std::cout << "Int: " << int_val.left() << "\n";
}

// Pattern matching
int_val.visit(overloaded{
    [](int i) { std::cout << "Int: " << i << "\n"; },
    [](const std::string& s) { std::cout << "String: " << s << "\n"; }
});
```

## PackedVariant

Discriminated union of multiple types.

### Interface

```cpp
template<typename... Ts>
class PackedVariant {
public:
    // Constructors
    template<size_t I, typename... Args>
    PackedVariant(std::in_place_index_t<I>, Args&&... args);

    // Queries
    size_t index() const;
    template<size_t I>
    bool holds_alternative() const;

    // Access
    template<size_t I>
    auto& get();
    template<size_t I>
    const auto& get() const;

    // Visitors
    template<typename F>
    auto visit(F&& visitor);
};
```

### Usage

```cpp
using Value = PackedVariant<
    Packed<int, EliasGamma>,
    PackedString,
    Packed<double, Float64>
>;

// Create
Value int_val(std::in_place_index<0>, 42);
Value str_val(std::in_place_index<1>, "hello");

// Check type
if (int_val.index() == 0) {
    std::cout << int_val.get<0>() << "\n";
}

// Exhaustive pattern matching
int_val.visit(overloaded{
    [](int i) { std::cout << "Int: " << i << "\n"; },
    [](const std::string& s) { std::cout << "String: " << s << "\n"; },
    [](double d) { std::cout << "Double: " << d << "\n"; }
});
```

## PackedResult

Represents success or error, commonly used for error handling.

### Interface

```cpp
template<typename T, typename E>
class PackedResult {
public:
    // Constructors
    static PackedResult ok(const T& value);
    static PackedResult err(const E& error);

    // Queries
    bool is_ok() const;
    bool is_err() const;
    explicit operator bool() const;

    // Access (throws if wrong state)
    T& unwrap();
    const T& unwrap() const;
    E& unwrap_err();
    const E& unwrap_err() const;

    // Safe access
    T unwrap_or(T default_value) const;
    template<typename F>
    T unwrap_or_else(F&& f) const;

    // Mapping
    template<typename F>
    auto map(F&& f) -> PackedResult<decltype(f(std::declval<T>())), E>;

    template<typename F>
    auto map_err(F&& f) -> PackedResult<T, decltype(f(std::declval<E>()))>;

    // Chaining
    template<typename F>
    auto and_then(F&& f);

    // Visitors
    template<typename F>
    auto visit(F&& visitor);
};
```

### Usage

```cpp
using Result = PackedResult<
    Packed<int, EliasGamma>,
    PackedString
>;

// Create results
auto success = Result::ok(42);
auto failure = Result::err("Invalid input");

// Check and access
if (success.is_ok()) {
    std::cout << "Value: " << success.unwrap() << "\n";
}

// Safe unwrap
int value = failure.unwrap_or(0);

// Error handling
auto result = parse_integer("42");
result.visit(overloaded{
    [](int value) { std::cout << "Parsed: " << value << "\n"; },
    [](const std::string& error) { std::cerr << "Error: " << error << "\n"; }
});

// Chaining
auto result2 = result
    .map([](int x) { return x * 2; })
    .and_then([](int x) { return validate(x); });
```

## PackedProduct

Tuple/struct of multiple values.

### Interface

```cpp
template<typename... Ts>
class PackedProduct {
public:
    // Constructors
    PackedProduct(const Ts&... values);

    // Access
    template<size_t I>
    auto& get();
    template<size_t I>
    const auto& get() const;

    // Structured binding support
    template<size_t I>
    auto& get() requires (I < sizeof...(Ts));
};
```

### Usage

```cpp
using Person = PackedProduct<
    PackedString,                    // name
    Packed<uint32_t, EliasGamma>,   // age
    PackedString                     // email
>;

// Create
Person person("Alice", 30, "alice@example.com");

// Access
std::cout << "Name: " << person.get<0>() << "\n";
std::cout << "Age: " << person.get<1>() << "\n";

// Structured binding (C++17)
auto& [name, age, email] = person;
std::cout << name << " is " << age << " years old\n";
```

## Helper Functions

```cpp
// Make optional
template<typename T>
auto make_optional(T value) -> PackedOptional<Packed<T>>;

// Make either
template<typename Either>
auto make_left(typename Either::left_type value) -> Either;

template<typename Either>
auto make_right(typename Either::right_type value) -> Either;

// Make result
template<typename Result>
auto make_ok(typename Result::value_type value) -> Result;

template<typename Result>
auto make_err(typename Result::error_type error) -> Result;

// Make product
template<typename... Ts>
auto make_product(Ts&&... values) -> PackedProduct<Packed<Ts>...>;
```

## Overloaded Pattern (Visitor Helper)

```cpp
template<typename... Fs>
struct overloaded : Fs... {
    using Fs::operator()...;
};

template<typename... Fs>
overloaded(Fs...) -> overloaded<Fs...>;
```

Usage with visit:

```cpp
variant.visit(overloaded{
    [](int i) { /* handle int */ },
    [](const std::string& s) { /* handle string */ },
    [](double d) { /* handle double */ }
});
```

## See Also

- [Packed Types](packed.md): Basic packed value types
- [Error Handling Guide](../guides/error-handling.md): Using Result types
- [Examples](../examples/algebraic.md): Practical examples
