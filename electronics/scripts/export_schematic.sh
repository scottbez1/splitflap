#!/bin/bash

set -e

eeschema $1 &
EESCHEMA_PID=$!

sleep 4

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
echo "Done"

kill $EESCHEMA_PID

