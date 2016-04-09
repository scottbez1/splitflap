#!/bin/bash

set -e

# Kill background jobs when the script terminates
trap 'jobs -pr | xargs --no-run-if-empty kill' SIGINT SIGTERM EXIT

SOURCE_PATH="$(cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")" && pwd)"

set -x

mkdir -p "$SOURCE_PATH/build"
recordmydesktop --no-sound --no-frame --on-the-fly-encoding -o "$SOURCE_PATH/build/export_schematic_screencast.ogv"&

eeschema "$SOURCE_PATH/splitflap.sch" &

sleep 5

echo "Focus main eeschema window"
xdotool search '\[' windowfocus

echo "Open File->Plot->Plot"
xdotool key "alt+f"
xdotool key p
xdotool key p

sleep 1

echo "Enter build output directory"
xdotool search 'Plot' windowfocus
xdotool type "build/"

sleep 1

echo "Plot"
xdotool key Tab Tab Tab Tab Tab Up Up Up space
xdotool key Return
sleep 4

echo "Rasterize..."
convert -density 96 "$SOURCE_PATH/build/splitflap.pdf" -background white -alpha remove "$SOURCE_PATH/build/schematic.png"

