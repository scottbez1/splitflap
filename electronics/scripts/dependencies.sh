#!/bin/bash
set -ev

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

sudo add-apt-repository --yes ppa:js-reynaud/kicad-4
sudo apt-get update -qq
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y kicad inkscape poppler-utils xdotool recordmydesktop

sudo pip install -r "$DIR/requirements.txt"
