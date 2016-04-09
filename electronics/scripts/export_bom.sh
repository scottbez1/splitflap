#!/bin/bash

#   Copyright 2016 Scott Bezek
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

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
wait $RECORDING_PID || true
wait $EESCHEMA_PID || true

