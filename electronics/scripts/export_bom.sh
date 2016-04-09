#!/bin/bash

set -e

# Kill background jobs when the script terminates
trap 'jobs -pr | xargs --no-run-if-empty kill' SIGINT SIGTERM EXIT

SOURCE_PATH="$(cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")" && pwd)"

set -x

mkdir -p "$SOURCE_PATH/build"
recordmydesktop --no-sound --no-frame --on-the-fly-encoding -o "$SOURCE_PATH/build/export_bom_screencast.ogv"&
RECORDING_PID=$!

eeschema "$SOURCE_PATH/splitflap.sch" &
EESCHEMA_PID=$!

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

kill $RECORDING_PID
kill $EESCHEMA_PID
wait $RECORDING_PID
wait $EESCHEMA_PID

