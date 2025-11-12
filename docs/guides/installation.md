# Installation

PFC is a header-only C++20 library, making installation straightforward. You have several options for integrating it into your project.

## Requirements

- C++20 compliant compiler:
    - GCC 10 or later
    - Clang 12 or later
    - MSVC 2019 (16.11) or later
- CMake 3.15+ (optional, for building tests and examples)

## Option 1: Header-Only Include

The simplest way to use PFC is to copy the headers directly into your project:

```bash
# Clone the repository
git clone https://github.com/spinozarabel/pfc.git

# Copy headers to your project
cp -r pfc/include/pfc /path/to/your/project/include/
```

Then include in your code:

```cpp
#include "pfc/pfc.hpp"
```

## Option 2: CMake Integration

### As a Subdirectory

Add PFC as a subdirectory in your CMake project:

```cmake
# In your CMakeLists.txt
add_subdirectory(external/pfc)

add_executable(your_app main.cpp)
target_link_libraries(your_app PRIVATE pfc)
```

### With CMake Install

Install PFC system-wide:

```bash
cd pfc
mkdir build && cd build
cmake ..
cmake --install . --prefix /usr/local
```

Then use in your project:

```cmake
find_package(pfc REQUIRED)

add_executable(your_app main.cpp)
target_link_libraries(your_app PRIVATE pfc::pfc)
```

## Option 3: Git Submodule

Add PFC as a git submodule:

```bash
cd your_project
git submodule add https://github.com/spinozarabel/pfc.git external/pfc
git submodule update --init --recursive
```

## Building Tests and Examples

To verify the installation and explore examples:

```bash
mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=20
cmake --build . -j4

# Run all tests
ctest --output-on-failure

# Or run individual test suites
./pfc_tests           # Core functionality
./test_advanced       # Advanced features
./test_production     # Production features (Huffman, LZ77, etc.)
./test_crc            # CRC checksums
./test_stream_arithmetic  # Stream I/O and arithmetic coding

# Run examples
./tutorial            # Basic tutorial
./advanced_demo       # Advanced features
```

### Building with Optional Examples

```bash
cmake .. -DBUILD_MISC_EXAMPLES=ON
cmake --build .

./simple_test
./examine_bytes
```

## Compiler Configuration

Ensure C++20 is enabled in your compiler:

### GCC/Clang

```bash
g++ -std=c++20 your_code.cpp -I/path/to/pfc/include
clang++ -std=c++20 your_code.cpp -I/path/to/pfc/include
```

### MSVC

```bash
cl /std:c++20 /I\path\to\pfc\include your_code.cpp
```

### CMake

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

## Optional Dependencies

PFC has no required dependencies, but can leverage optional libraries for enhanced functionality:

- **Intel TBB**: Enables parallel algorithm execution
    - Install: `apt install libtbb-dev` (Ubuntu/Debian)
    - Or: `brew install tbb` (macOS)
    - CMake will automatically detect and use if available

## Verification

Create a simple test file to verify installation:

```cpp
// test_pfc.cpp
#include <pfc/pfc.hpp>
#include <iostream>
#include <vector>

int main() {
    using namespace pfc;

    // Test basic compression
    std::vector<uint32_t> data = {1, 2, 3, 5, 8, 13};
    auto compressed = compress<EliasGamma>(data);
    auto decompressed = decompress<EliasGamma, uint32_t>(compressed);

    if (data == decompressed) {
        std::cout << "PFC is working correctly!\n";
        std::cout << "Compressed " << data.size() * sizeof(uint32_t)
                  << " bytes to " << compressed.size() << " bytes\n";
        return 0;
    } else {
        std::cerr << "Error: Data mismatch\n";
        return 1;
    }
}
```

Compile and run:

```bash
g++ -std=c++20 test_pfc.cpp -I/path/to/pfc/include -o test_pfc
./test_pfc
```

Expected output:
```
PFC is working correctly!
Compressed 24 bytes to 7 bytes
```

## Troubleshooting

### Compiler Errors

If you see errors about missing C++20 features:

1. Verify your compiler version supports C++20
2. Ensure `-std=c++20` or equivalent is set
3. Check that C++20 concepts are supported

### CMake Issues

If CMake can't find the package:

```bash
# Specify the installation prefix
cmake .. -DCMAKE_PREFIX_PATH=/path/to/pfc/install
```

### Include Path Problems

Make sure the include path points to the directory containing the `pfc` folder:

```
project/
  include/
    pfc/          <- Point here
      pfc.hpp
      core.hpp
      ...
```

## Next Steps

- [Quick Start Guide](quick-start.md): Learn the basics
- [API Reference](../api/overview.md): Explore available components
- [Examples](../examples/basic.md): See real usage patterns
