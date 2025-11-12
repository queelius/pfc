#!/bin/bash
# Build script for PFC whitepaper

set -e

echo "Building PFC Whitepaper..."
echo "=========================="

# Check for pdflatex
if ! command -v pdflatex &> /dev/null; then
    echo "Error: pdflatex not found. Please install a LaTeX distribution."
    echo ""
    echo "Installation instructions:"
    echo "  Ubuntu/Debian: sudo apt-get install texlive-latex-base texlive-latex-extra"
    echo "  Fedora/RHEL:   sudo dnf install texlive-scheme-medium"
    echo "  macOS:         brew install --cask mactex"
    exit 1
fi

# Navigate to papers directory
cd "$(dirname "$0")"

# Clean previous build artifacts
echo "Cleaning previous build..."
rm -f *.aux *.log *.bbl *.blg *.out *.toc *.lof *.lot *.fls *.fdb_latexmk *.synctex.gz

# First pass
echo "Running pdflatex (pass 1/3)..."
pdflatex -interaction=nonstopmode whitepaper.tex > /dev/null 2>&1 || {
    echo "Error in first pdflatex pass. Check whitepaper.log for details."
    exit 1
}

# BibTeX (if available and needed)
if command -v bibtex &> /dev/null && [ -f whitepaper.aux ]; then
    echo "Running bibtex..."
    bibtex whitepaper > /dev/null 2>&1 || true  # Don't fail if no bibliography
fi

# Second pass
echo "Running pdflatex (pass 2/3)..."
pdflatex -interaction=nonstopmode whitepaper.tex > /dev/null 2>&1 || {
    echo "Error in second pdflatex pass. Check whitepaper.log for details."
    exit 1
}

# Third pass (for references)
echo "Running pdflatex (pass 3/3)..."
pdflatex -interaction=nonstopmode whitepaper.tex > /dev/null 2>&1 || {
    echo "Error in third pdflatex pass. Check whitepaper.log for details."
    exit 1
}

# Check output
if [ -f whitepaper.pdf ]; then
    SIZE=$(du -h whitepaper.pdf | cut -f1)
    PAGES=$(pdfinfo whitepaper.pdf 2>/dev/null | grep "Pages:" | awk '{print $2}')
    echo ""
    echo "Success! Generated whitepaper.pdf"
    echo "  Size: $SIZE"
    if [ -n "$PAGES" ]; then
        echo "  Pages: $PAGES"
    fi
    echo ""
    echo "Location: $(pwd)/whitepaper.pdf"
else
    echo "Error: PDF was not generated"
    exit 1
fi

# Optional: open PDF viewer
if [ "$1" = "--view" ] || [ "$1" = "-v" ]; then
    echo "Opening PDF viewer..."
    if command -v xdg-open &> /dev/null; then
        xdg-open whitepaper.pdf
    elif command -v open &> /dev/null; then
        open whitepaper.pdf
    elif command -v evince &> /dev/null; then
        evince whitepaper.pdf &
    else
        echo "No PDF viewer found. Please open whitepaper.pdf manually."
    fi
fi
