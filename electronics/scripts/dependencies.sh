#!/bin/bash
set -ev

sudo add-apt-repository --yes ppa:js-reynaud/kicad-4
sudo apt-get update -qq
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y kicad inkscape poppler-utils

