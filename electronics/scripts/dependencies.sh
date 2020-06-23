#!/bin/bash
set -ev

sudo add-apt-repository --yes ppa:js-reynaud/kicad-5
sudo apt-get update -qq
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y kicad inkscape poppler-utils xdotool recordmydesktop
mkdir -p ~/.config/kicad
cp /usr/share/kicad/template/fp-lib-table ~/.config/kicad/
cp /usr/share/kicad/template/sym-lib-table ~/.config/kicad/
echo "canvas_type=1" > ~/.config/kicad/pcbnew

