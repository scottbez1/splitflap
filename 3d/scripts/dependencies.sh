#!/bin/bash

set -ev

sudo add-apt-repository --yes ppa:openscad/releases
sudo apt-get update -qq
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y openscad inkscape imagemagick xvfb
sudo apt-get purge -y fglrx
