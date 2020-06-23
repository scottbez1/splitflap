#!/bin/bash
set -ev

sudo add-apt-repository --yes ppa:js-reynaud/kicad-5
sudo apt-get update -qq
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y kicad inkscape poppler-utils xdotool recordmydesktop python3-dev python3-pip

sudo python3 -m pip install psutil

mkdir -p ~/.config/kicad
cp /usr/share/kicad/template/fp-lib-table ~/.config/kicad/
cp /usr/share/kicad/template/sym-lib-table ~/.config/kicad/
echo "canvas_type=1" > ~/.config/kicad/pcbnew
echo "SMaskColor_Red=0.1" > ~/.config/kicad/pcbnew
echo "SMaskColor_Green=0.1" > ~/.config/kicad/pcbnew
echo "SMaskColor_Blue=0.1" > ~/.config/kicad/pcbnew
echo "RenderEngine=1" > ~/.config/kicad/pcbnew
echo "Render_RAY_ProceduralTextures=1" > ~/.config/kicad/pcbnew

