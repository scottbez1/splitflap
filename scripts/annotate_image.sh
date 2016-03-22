#!/bin/bash

set -ev

LABEL="`date --rfc-3339=seconds`\n`git rev-parse --short HEAD`"
convert -background black -fill white -pointsize 12 -font Ubuntu-Mono-Bold label:"$LABEL" -bordercolor black -border 3 $1 +swap -append $1

