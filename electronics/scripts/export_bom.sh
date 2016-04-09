#!/bin/bash

set -e

# Kill background jobs when the script terminates
trap 'jobs -pr | xargs --no-run-if-empty kill' SIGINT SIGTERM EXIT

SOURCE_PATH="$(cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")" && pwd)"

set -x

mkdir -p "$SOURCE_PATH/build"
recordmydesktop --no-sound --no-frame --on-the-fly-encoding -o "$SOURCE_PATH/build/export_bom_screencast.ogv"&

eeschema "$SOURCE_PATH/splitflap.sch" &

sleep 5

echo "Focus main eeschema window"
xdotool search '\[' windowfocus

echo "Open Tools->Generate Bill Of Materials"
xdotool key "alt+t"
xdotool key m

sleep 1

#xdotool search --name '.*' getwindowname %@

echo "Run generate"
xdotool search 'Bill of Material' windowfocus
xdotool key Return

sleep 4

