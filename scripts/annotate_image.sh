#!/bin/bash

set -ev

LABEL="`date --rfc-3339=seconds`\n`git rev-parse --short HEAD`"
convert $1 -background Black -fill White label:"$LABEL" -gravity West -append $1

