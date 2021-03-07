#!/bin/bash
set -e

if [[ -z "${GITHUB_WORKFLOW}" ]]; then
    >&2 echo "Aborting! This script is meant to be run in CI (Github Actions) only. It may modify/damage your system configuration if run outside of CI."
    exit 1
fi

set -v

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

sudo add-apt-repository --yes ppa:kicad/kicad-5.1-releases
sudo apt-get update -qq
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y kicad inkscape poppler-utils xdotool recordmydesktop python3-dev python3-pip

sudo python3 -m pip install psutil

mkdir -p ~/.config/kicad
cp /usr/share/kicad/template/fp-lib-table ~/.config/kicad/
cp /usr/share/kicad/template/sym-lib-table ~/.config/kicad/

cp "$DIR/config/eeschema" ~/.config/kicad/
cp "$DIR/config/pcbnew" ~/.config/kicad/

# Install ImageMagick policy that allows PDF conversion (safe in CI because we control all inputs/outputs)
sudo cp "$DIR/config/policy.xml" /etc/ImageMagick-6/policy.xml

# eeschema default settings (pdf export)
# echo "PlotFormat=4" >> ~/.config/kicad/eeschema

# # pcbnew default settings (for 3d rendering)
# echo "canvas_type=1" >> ~/.config/kicad/pcbnew
# echo "SMaskColor_Red=0.1" >> ~/.config/kicad/pcbnew
# echo "SMaskColor_Green=0.1" >> ~/.config/kicad/pcbnew
# echo "SMaskColor_Blue=0.1" >> ~/.config/kicad/pcbnew
# echo "RenderEngine=1" >> ~/.config/kicad/pcbnew
# echo "Render_RAY_ProceduralTextures=0" >> ~/.config/kicad/pcbnew

