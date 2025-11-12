# Makefile for building PFC whitepaper

# Main targets
.PHONY: all clean view

PAPER = whitepaper
TEX = $(PAPER).tex
PDF = $(PAPER).pdf
BIB = references.bib

# LaTeX compilation commands
LATEX = pdflatex
BIBTEX = bibtex

# Build the PDF
all: $(PDF)

# Full build with bibliography
$(PDF): $(TEX) $(BIB)
	$(LATEX) $(PAPER)
	$(BIBTEX) $(PAPER) || true
	$(LATEX) $(PAPER)
	$(LATEX) $(PAPER)

# Quick build without bibliography updates
quick: $(TEX)
	$(LATEX) $(PAPER)

# Clean auxiliary files
clean:
	rm -f *.aux *.log *.bbl *.blg *.out *.toc *.lof *.lot *.fls *.fdb_latexmk *.synctex.gz

# Clean everything including PDF
distclean: clean
	rm -f $(PDF)

# View the PDF (requires a PDF viewer)
view: $(PDF)
	@if command -v xdg-open > /dev/null; then \
		xdg-open $(PDF); \
	elif command -v open > /dev/null; then \
		open $(PDF); \
	elif command -v evince > /dev/null; then \
		evince $(PDF); \
	else \
		echo "No PDF viewer found. Please open $(PDF) manually."; \
	fi

# Help target
help:
	@echo "PFC Whitepaper Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build the complete PDF with bibliography (default)"
	@echo "  quick     - Quick build without bibliography updates"
	@echo "  clean     - Remove auxiliary files"
	@echo "  distclean - Remove all generated files including PDF"
	@echo "  view      - Build and open the PDF"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Requirements:"
	@echo "  - pdflatex (from TeX Live or MiKTeX)"
	@echo "  - bibtex (for bibliography)"
	@echo ""
	@echo "Example usage:"
	@echo "  make          # Build the paper"
	@echo "  make view     # Build and view"
	@echo "  make clean    # Clean up temporary files"
