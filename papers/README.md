# PFC Whitepaper

This directory contains the comprehensive whitepaper on the PFC (Prefix-Free Codecs) library.

## Files

- `whitepaper.tex` - Main LaTeX source document
- `references.bib` - BibTeX bibliography file
- `Makefile` - Build system for generating the PDF
- `README.md` - This file

## Building the PDF

### Prerequisites

You need a LaTeX distribution installed:

**Linux (Debian/Ubuntu):**
```bash
sudo apt-get install texlive-latex-base texlive-latex-extra texlive-fonts-recommended
```

**Linux (Fedora/RHEL):**
```bash
sudo dnf install texlive-scheme-medium
```

**macOS:**
```bash
brew install --cask mactex
# Or use BasicTeX for a smaller installation:
brew install --cask basictex
```

**Windows:**
- Install [MiKTeX](https://miktex.org/download) or [TeX Live](https://www.tug.org/texlive/)

### Build Commands

**Full build (recommended):**
```bash
make
```

This runs pdflatex and bibtex multiple times to resolve all references and citations.

**Quick build (no bibliography updates):**
```bash
make quick
```

**Build and view:**
```bash
make view
```

**Clean temporary files:**
```bash
make clean
```

**Remove all generated files:**
```bash
make distclean
```

### Manual Build

If you don't have `make`, build manually:

```bash
pdflatex whitepaper
bibtex whitepaper
pdflatex whitepaper
pdflatex whitepaper
```

The PDF will be generated as `whitepaper.pdf`.

## Document Structure

The whitepaper covers:

1. **Introduction & Motivation** - The problem of data compression vs. runtime efficiency
2. **Prefix-Free Codes** - Foundation of the zero-copy invariant
3. **Core Architecture** - Bit I/O, codecs, and packed values
4. **Algebraic Type System** - Product types, sum types, recursive structures
5. **Zero-Copy Iteration** - STL integration with proxy values
6. **Numeric Codecs** - Floating point, rationals, complex numbers
7. **Type Erasure** - Runtime polymorphism when needed
8. **Algorithms** - Zero-copy transforms and parallel processing
9. **Implementation** - Techniques for efficiency and expressiveness
10. **Performance Analysis** - Compression ratios and runtime characteristics
11. **Related Work** - Universal coding, compression libraries, generic programming
12. **Future Work** - Limitations and extensions
13. **Conclusion** - Summary of contributions

## Technical Content

The whitepaper demonstrates:

- **Zero-copy invariant**: In-memory representation equals wire representation
- **Compression ratios**: 3-10× for typical integer distributions
- **Full type algebra**: Products, sums, recursive types in compressed form
- **STL compatibility**: Standard algorithms work on compressed containers
- **Generic programming**: Stepanov's principles applied to compression

## Citation

If you use this work, please cite:

```bibtex
@techreport{pfc2024,
  title = {PFC: Zero-Copy Data Compression Through Prefix-Free Codecs
           and Generic Programming},
  author = {PFC Library Authors},
  year = {2024},
  url = {https://github.com/spinoza/pfc}
}
```

## License

The whitepaper and all accompanying materials are provided under the same license as the PFC library (MIT License).
