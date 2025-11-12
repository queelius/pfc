# Basic Usage Examples

This page demonstrates fundamental PFC usage patterns for common tasks.

## Simple Compression

Compress and decompress a vector of integers:

```cpp
#include <pfc/pfc.hpp>
#include <vector>
#include <iostream>

using namespace pfc;

int main() {
    // Original data
    std::vector<uint32_t> data = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89};

    // Compress using default codec (EliasGamma)
    auto compressed = compress(data);

    // Decompress
    auto decompressed = decompress<uint32_t>(compressed);

    // Verify
    assert(data == decompressed);

    // Show compression ratio
    std::cout << "Original: " << data.size() * sizeof(uint32_t) << " bytes\n";
    std::cout << "Compressed: " << compressed.size() << " bytes\n";
    std::cout << "Ratio: " << (data.size() * sizeof(uint32_t)) / (double)compressed.size() << "x\n";
}
```

**Output**:
```
Original: 40 bytes
Compressed: 12 bytes
Ratio: 3.33x
```

## Choosing Different Codecs

Different codecs work better for different data distributions:

```cpp
#include <pfc/pfc.hpp>
#include <random>

using namespace pfc;

int main() {
    std::mt19937 gen(42);

    // Small numbers - use Elias Gamma
    std::geometric_distribution<> small_dist(0.3);
    std::vector<uint32_t> small_nums;
    for (int i = 0; i < 1000; ++i) {
        small_nums.push_back(small_dist(gen));
    }
    auto compressed_gamma = compress<EliasGamma>(small_nums);

    // Medium numbers - use Elias Delta
    std::uniform_int_distribution<uint32_t> med_dist(0, 100000);
    std::vector<uint32_t> med_nums;
    for (int i = 0; i < 1000; ++i) {
        med_nums.push_back(med_dist(gen));
    }
    auto compressed_delta = compress<EliasDelta>(med_nums);

    // Compare
    std::cout << "Small nums with Gamma: " << compressed_gamma.size() << " bytes\n";
    std::cout << "Medium nums with Delta: " << compressed_delta.size() << " bytes\n";
}
```

## Working with Packed Values

Packed values maintain compressed representation:

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

int main() {
    // Define a packed integer type
    using PackedInt = Packed<uint32_t, EliasGamma>;

    // Create packed values
    PackedInt a(42);
    PackedInt b(100);

    // Values are stored in compressed form
    std::cout << "Packed size of a: " << a.byte_size() << " bytes\n";
    std::cout << "Packed size of b: " << b.byte_size() << " bytes\n";

    // Decode on access
    uint32_t decoded_a = a.get();
    uint32_t decoded_b = b.get();

    // Comparison works directly
    assert(a != b);

    // Can be copied efficiently
    PackedInt c = a;
    assert(c == a);
}
```

## Packed Containers

Use PackedVector for dynamic collections:

```cpp
#include <pfc/pfc.hpp>
#include <algorithm>
#include <numeric>

using namespace pfc;

int main() {
    // Create a packed vector
    PackedVector<Packed<uint32_t, EliasGamma>> vec;

    // Add elements
    for (uint32_t i = 1; i <= 10; ++i) {
        vec.push_back(i * i);  // 1, 4, 9, 16, 25, ...
    }

    // Access elements (decoded on demand)
    for (const auto& value : vec) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    // Works with STL algorithms
    auto sum = std::accumulate(vec.begin(), vec.end(), 0u);
    std::cout << "Sum: " << sum << "\n";

    // Sorting
    std::sort(vec.begin(), vec.end(), std::greater<>());

    // Finding
    auto it = std::find(vec.begin(), vec.end(), 25);
    if (it != vec.end()) {
        std::cout << "Found 25 at index " << std::distance(vec.begin(), it) << "\n";
    }
}
```

## Strings

PackedString provides compressed string storage:

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

int main() {
    // Create strings
    PackedString str1("Hello, PFC!");
    PackedString str2("Compressed strings");

    // Conversion
    std::string std_str = str1.to_string();

    // Comparison
    assert(str1 != str2);

    // Concatenation (via conversion)
    PackedString str3(str1.to_string() + " " + str2.to_string());

    // Access characters
    for (size_t i = 0; i < str1.size(); ++i) {
        std::cout << str1[i];
    }
    std::cout << "\n";
}
```

## Signed Integers

Use Signed codec wrapper for negative values:

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

int main() {
    // Define signed packed type
    using SignedInt = Packed<int, Signed<EliasGamma>>;

    // Create values
    SignedInt positive(42);
    SignedInt negative(-42);
    SignedInt zero(0);

    // All efficiently encoded using zigzag
    std::cout << "Positive size: " << positive.byte_size() << " bytes\n";
    std::cout << "Negative size: " << negative.byte_size() << " bytes\n";
    std::cout << "Zero size: " << zero.byte_size() << " bytes\n";

    // Vector of signed integers
    PackedVector<SignedInt> vec = {-10, -5, 0, 5, 10};

    // Works naturally with signed arithmetic
    for (auto val : vec) {
        std::cout << val << " ";
    }
    std::cout << "\n";
}
```

## File I/O

Save and load packed data:

```cpp
#include <pfc/pfc.hpp>
#include <fstream>

using namespace pfc;

void save_data(const std::string& filename,
               const PackedVector<Packed<uint32_t, EliasGamma>>& vec) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file");
    }

    // Write size
    uint32_t size = vec.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

    // Write packed data directly
    file.write(reinterpret_cast<const char*>(vec.data()), vec.byte_size());
}

PackedVector<Packed<uint32_t, EliasGamma>> load_data(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file");
    }

    // Read size
    uint32_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));

    // Read packed data
    std::vector<uint8_t> buffer(size * 10);  // Estimate
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

    // Construct from buffer
    return PackedVector<Packed<uint32_t, EliasGamma>>(buffer.data(), buffer.size());
}

int main() {
    // Create and save
    PackedVector<Packed<uint32_t, EliasGamma>> vec = {1, 2, 3, 5, 8, 13};
    save_data("data.bin", vec);

    // Load
    auto loaded = load_data("data.bin");

    // Verify
    assert(vec == loaded);
}
```

## Network Transmission

Send packed data over network:

```cpp
#include <pfc/pfc.hpp>
#include <sys/socket.h>

using namespace pfc;

void send_packed_data(int socket, const PackedVector<Packed<uint32_t, EliasGamma>>& vec) {
    // Send size header
    uint32_t size = vec.byte_size();
    send(socket, &size, sizeof(size), 0);

    // Send packed data directly (zero-copy)
    send(socket, vec.data(), size, 0);
}

PackedVector<Packed<uint32_t, EliasGamma>> receive_packed_data(int socket) {
    // Receive size
    uint32_t size;
    recv(socket, &size, sizeof(size), MSG_WAITALL);

    // Receive data
    std::vector<uint8_t> buffer(size);
    recv(socket, buffer.data(), size, MSG_WAITALL);

    // Zero-copy construction
    return PackedVector<Packed<uint32_t, EliasGamma>>(buffer.data(), buffer.size());
}
```

## Configuration Storage

Store configuration with mixed types:

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

struct Config {
    PackedVector<Packed<uint32_t, EliasGamma>> allowed_ports;
    PackedString server_name;
    Packed<uint32_t, FixedWidth<32>> flags;
    Packed<float, Float32> timeout;
};

int main() {
    // Create configuration
    Config config;
    config.allowed_ports = {80, 443, 8080};
    config.server_name = PackedString("MyServer");
    config.flags = Packed<uint32_t, FixedWidth<32>>(0xFF00);
    config.timeout = Packed<float, Float32>(30.0f);

    // Each field maintains compressed representation
    std::cout << "Ports size: " << config.allowed_ports.byte_size() << " bytes\n";
    std::cout << "Name size: " << config.server_name.byte_size() << " bytes\n";

    // Access values
    for (auto port : config.allowed_ports) {
        std::cout << "Port: " << port << "\n";
    }
    std::cout << "Server: " << config.server_name.to_string() << "\n";
    std::cout << "Timeout: " << config.timeout.get() << "s\n";
}
```

## Bit-Level Operations

Low-level bit manipulation:

```cpp
#include <pfc/pfc.hpp>

using namespace pfc;

int main() {
    uint8_t buffer[64] = {};

    // Writing
    {
        BitWriter writer(buffer, sizeof(buffer));

        // Write individual bits
        writer.write_bit(1);
        writer.write_bit(0);
        writer.write_bit(1);

        // Write multiple bits
        writer.write_bits(0b1010, 4);

        // Write with different codecs
        EliasGamma::encode(42, writer);
        EliasDelta::encode(1000, writer);

        // Align to byte boundary
        writer.align();

        std::cout << "Wrote " << writer.bits_written() << " bits\n";
    }

    // Reading
    {
        BitReader reader(buffer, sizeof(buffer));

        // Read individual bits
        bool b1 = reader.read_bit();
        bool b2 = reader.read_bit();
        bool b3 = reader.read_bit();

        // Read multiple bits
        uint32_t nibble = reader.read_bits(4);

        // Read with codecs
        auto v1 = EliasGamma::decode(reader);
        auto v2 = EliasDelta::decode(reader);

        std::cout << "Read values: " << v1 << ", " << v2 << "\n";
    }
}
```

## Next Steps

- [Codec Examples](codecs.md): Detailed codec usage
- [Container Examples](containers.md): Advanced container operations
- [Algebraic Types Examples](algebraic.md): Sum and product types
- [Compression Examples](compression.md): Huffman, LZ77
