#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

set -v

sudo apt-get update -qq
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y openscad inkscape imagemagick xvfb texlive-extra-utils

sudo pip install -r "$DIR/requirements.txt"
