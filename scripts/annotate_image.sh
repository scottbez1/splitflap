#!/bin/bash

set -e

FONT="3d/roboto/RobotoMono-Bold.ttf"

# Make sure font exists
ls "$FONT"

set -v

LABEL="`date --rfc-3339=seconds`\n`git rev-parse --short HEAD`"
convert -background black -fill white -pointsize 12 -font "$FONT" label:"$LABEL" -bordercolor black -border 3 $1 +swap -append $1

