#!/bin/bash

set -ev

mkdir -p deploy

cp 3d/build/animation/animation.gif deploy/3d_animation.gif

cp 3d/build/laser_parts/combined.svg deploy/3d_laser_vector.svg
cp 3d/build/laser_parts/raster.png deploy/3d_laser_raster.png
./scripts/annotate_image.sh deploy/3d_laser_raster.png

cp -r 3d/build/colored_stl deploy/3d_colored_stl

cp electronics/build/splitflap.pdf deploy/schematic.pdf
cp electronics/build/schematic.png deploy/schematic.png
cp electronics/build/bom.csv deploy/bom.csv

# Control board
cp electronics/build/splitflap_merged.png deploy/pcb_raster.png
./scripts/annotate_image.sh deploy/pcb_raster.png
cp electronics/build/splitflap_packet.pdf deploy/pcb_packet.pdf
cp electronics/build/splitflap_gerber.zip deploy/pcb_gerber.zip

# Panelized control board
cp electronics/build/panelized_splitflap_merged.png deploy/panelized_pcb_raster.png
./scripts/annotate_image.sh deploy/panelized_pcb_raster.png
cp electronics/build/panelized_splitflap_packet.pdf deploy/panelized_pcb_packet.pdf
cp electronics/build/panelized_splitflap_gerber.zip deploy/panelized_pcb_gerber.zip

# Panelized sensor board
cp electronics/build/panelized_sensor_merged.png deploy/sensor_pcb_raster.png
./scripts/annotate_image.sh deploy/sensor_pcb_raster.png
cp electronics/build/panelized_sensor_packet.pdf deploy/sensor_pcb_packet.pdf
cp electronics/build/panelized_sensor_gerber.zip deploy/sensor_pcb_gerber.zip
