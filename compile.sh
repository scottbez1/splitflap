#!/bin/bash

set -e
#set -x

DATE_RAW=`git show -s --format=%ci HEAD`
DATE=`date --date "$DATE_RAW" +%F`
REV=`git rev-parse --short HEAD`

echo "Render cut layer..."
openscad -o build/cut.svg -D 'exploded=true' -D 'export=true' -D 'render_layers=[0]' -D "date=\"$DATE\"" -D "revision=\"$REV\"" enclosure.scad

echo "Render etch layer..."
openscad -o build/etch.svg -D 'exploded=true' -D 'export=true' -D 'render_layers=[1]' -D "date=\"$DATE\"" -D "revision=\"$REV\"" enclosure.scad

echo "Post process..."
python -u postProcess.py
