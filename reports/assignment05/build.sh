#!/usr/bin/env bash
set -euo pipefail

ASSIGNMENT="assignment05" 

# uncomment the line below, to delete the temporary inkscape files. compiling 
# svg can take a while, but automatically deleting them can make debuggin easier
# rm -rf svg-inkscape

# run once to precompile
lualatex -shell-escape "$ASSIGNMENT.tex"

# run biber (biblatex)
biber $ASSIGNMENT

# run twice to resolve references
lualatex -shell-escape "$ASSIGNMENT.tex"
lualatex -shell-escape "$ASSIGNMENT.tex"
