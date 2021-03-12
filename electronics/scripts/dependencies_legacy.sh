#!/bin/bash
set -ev

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

sudo add-apt-repository --yes ppa:js-reynaud/kicad-4
sudo apt-get update -qq
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y kicad inkscape poppler-utils xdotool recordmydesktop

# Install ImageMagick policy that allows PDF conversion (safe in CI because we control all inputs/outputs)
sudo cp "$DIR/config/policy.xml" /etc/ImageMagick-6/policy.xml