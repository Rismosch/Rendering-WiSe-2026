#!/usr/bin/env bash
set -euo pipefail

# uncomment the line below, to delete the temporary inkscape files. compiling 
# svg can take a while, but automatically deleting them can make debuggin easier
# rm -rf svg-inkscape

# run twice to resolve references
lualatex -shell-escape assignment02.tex
lualatex -shell-escape assignment02.tex
