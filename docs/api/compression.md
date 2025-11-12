# Compression API

Advanced compression algorithms beyond basic codecs.

## Huffman Coding

```cpp
#include <pfc/huffman.hpp>

// Build frequency table
FrequencyTable freq_table;
for (uint8_t byte : data) {
    freq_table.add(byte);
}

// Create Huffman tree
auto huffman_tree = build_huffman_tree(freq_table);

// Compress
std::vector<uint8_t> compressed = huffman_encode(data, huffman_tree);

// Decompress
std::vector<uint8_t> decompressed = huffman_decode(compressed, huffman_tree);
```

## LZ77

```cpp
#include <pfc/lz77.hpp>

// Compress with LZ77
LZ77Encoder encoder;
auto compressed = encoder.compress(data);

// Decompress
LZ77Decoder decoder;
auto decompressed = decoder.decompress(compressed);
```

## Arithmetic Coding

```cpp
#include <pfc/arithmetic_coding.hpp>

// Create probability model
ProbabilityModel model;
// ... configure model ...

// Encode
ArithmeticEncoder encoder(model);
auto compressed = encoder.encode(data);

// Decode
ArithmeticDecoder decoder(model);
auto decompressed = decoder.decode(compressed);
```

See [Compression Examples](../examples/compression.md) for detailed usage.
