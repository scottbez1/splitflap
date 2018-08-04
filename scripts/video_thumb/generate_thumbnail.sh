#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

VIDEO_ID="vq4o_88kN8g"

curl https://img.youtube.com/vi/$VIDEO_ID/maxresdefault.jpg | convert - -resize 400x "$DIR/play_button.png" -gravity center -composite -format jpg -quality 90 "$DIR/../../renders/prototypeVideoThumbnail.jpg"