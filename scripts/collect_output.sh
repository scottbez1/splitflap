#!/bin/bash

set -ev

mkdir -p deploy

cp 3d/build/animation/animation.gif deploy/3d_animation.gif

cp 3d/build/laser_parts/combined.svg deploy/3d_laser_vector.svg
cp 3d/build/laser_parts/raster.png deploy/3d_laser_raster.png
./scripts/annotate_image.sh deploy/3d_laser_raster.png

cp electronics/build/merged.png deploy/pcb_raster.png
./scripts/annotate_image.sh deploy/pcb_raster.png

cp electronics/build/splitflap_packet.pdf deploy/pcb_packet.pdf
cp electronics/build/splitflap_gerber.zip deploy/pcb_gerber.zip
cp electronics/build/splitflap.pdf deploy/schematic.pdf
cp electronics/build/schematic.png deploy/schematic.png
cp electronics/build/bom.csv deploy/bom.csv

