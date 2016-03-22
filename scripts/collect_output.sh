#!/bin/bash

set -ev

mkdir -p build

cp 3d/build/animation/animation.gif build/3d_animation.gif

cp 3d/build/laser_parts/raster.png build/3d_laser_raster.png
./scripts/annotate_image.sh build/3d_laser_raster.png

cp electronics/build/merged.png build/pcb_raster.png
./scripts/annotate_image.sh build/pcb_raster.png

