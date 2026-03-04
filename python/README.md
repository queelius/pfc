# PFC Python Bindings

Python bindings for the [PFC (Prefix-Free Codecs)](https://github.com/queelius/pfc) C++20 library.

## Features

- **Zero-copy data compression** using prefix-free codes
- **Rich codec library**: Elias Gamma/Delta/Omega, Fibonacci, Rice, VByte, ExpGolomb
- **Bit-level I/O**: Fine-grained control over encoding/decoding
- **NumPy integration**: Work with arrays efficiently
- **High-level API**: Simple compress/decompress functions

## Installation

### From Source

```bash
# Clone the PFC repository (bindings are included)
git clone https://github.com/queelius/pfc.git
cd pfc/python

# Build with pip
pip install .

# Or for development
pip install -e .
```

### Requirements

- Python >= 3.9
- C++20 compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake >= 3.15
- pybind11 >= 2.11
- NumPy >= 1.20

## Quick Start

```python
import pfc

# High-level API: compress a list of integers
data = [1, 2, 3, 5, 8, 13, 21, 34]
compressed = pfc.compress(data, codec='elias_gamma')
decompressed = pfc.decompress(compressed, codec='elias_gamma')
assert data == list(decompressed)

# Bit-level API
writer = pfc.BitWriter(1024)
writer.write(True)
writer.write_bits(42, 8)
writer.align()

reader = pfc.BitReader(writer.to_bytes())
bit = reader.read()           # True
value = reader.read_bits(8)   # 42
```

## Available Codecs

| Codec | Name | Description |
|-------|------|-------------|
| Elias Gamma | `elias_gamma` | Fundamental universal code |
| Elias Delta | `elias_delta` | More efficient for large numbers |
| Fibonacci | `fibonacci` | Based on Fibonacci sequence |
| Unary | `unary` | Simplest variable-length code |
| Rice | `rice_k` | Parameterized (k=0..8) |
| ExpGolomb | `exp_golomb_n` | Parameterized (order=0..4) |
| Fixed | `fixed_w` | Fixed-width (w=8,16,32,64) |

### Signed Integers

```python
from pfc import codecs

writer = pfc.BitWriter(1024)
codecs.signed_gamma.encode(-42, writer)
writer.align()

reader = pfc.BitReader(writer.to_bytes())
value = codecs.signed_gamma.decode(reader)  # -42
```

### Parameterized Codecs

```python
from pfc import codecs

# Rice codec with k=3
rice = codecs.rice(k=3)

# ExpGolomb with order=2
expg = codecs.exp_golomb(order=2)

# Fixed-width 16-bit
fixed16 = codecs.fixed(width=16)
```

## Development

```bash
cd pfc/python

# Install development dependencies
pip install -e ".[dev]"

# Run tests
pytest

# Type checking
mypy src/pfc_python

# Format code
black src tests
ruff check src tests
```

## License

MIT License - see the repository [LICENSE](../LICENSE) file for details.

## Documentation

- [PFC Documentation](https://queelius.github.io/pfc/) - Full library documentation
