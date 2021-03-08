#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

set -v

sudo add-apt-repository --yes ppa:openscad/releases
sudo apt-get update -qq
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y openscad inkscape imagemagick xvfb
sudo apt-get purge -y fglrx

sudo pip install -r "$DIR/requirements.txt"
