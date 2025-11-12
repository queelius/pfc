# CRC API

Cyclic Redundancy Check for data integrity.

```cpp
#include <pfc/crc.hpp>
```

## CRC8

```cpp
uint8_t checksum = crc8(data.data(), data.size());
```

## CRC16

```cpp
// CRC-16-CCITT
uint16_t checksum_ccitt = crc16_ccitt(data.data(), data.size());

// CRC-16-IBM
uint16_t checksum_ibm = crc16_ibm(data.data(), data.size());
```

## CRC32

```cpp
// IEEE 802.3 / Ethernet
uint32_t checksum = crc32(data.data(), data.size());
```

## Incremental CRC

```cpp
CRC32 crc;
for (const auto& chunk : chunks) {
    crc.update(chunk.data(), chunk.size());
}
uint32_t final_checksum = crc.finalize();
```

## Usage Example

```cpp
#include <pfc/crc.hpp>

// Add checksum to data
PackedVector<Packed<uint32_t, EliasGamma>> data = {1, 2, 3, 4, 5};
uint32_t checksum = crc32(data.data(), data.byte_size());

// Verify on receive
uint32_t received_checksum = crc32(received_data.data(),
                                   received_data.byte_size());
if (checksum == received_checksum) {
    std::cout << "Data integrity verified\n";
} else {
    std::cerr << "Data corrupted!\n";
}
```

See [API Overview](overview.md) for complete documentation.
