#!/usr/bin/env python
#   Copyright 2015-2018 Scott Bezek and the splitflap contributors
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

import os

ORIGIN = (20, 20)
BOARD_SIZE = (16.256, 16.256)

ROWS = 4
COLS = 5

VCUT_LINE_OUTSET = 2
VCUT_LABEL_OUTSET = (6, 3)

_HEADER = """
load sensor.kicad_pcb
create-template
set-title "Splitflap sensor board [PANELIZED]"
set-comment-1 "https://github.com/scottbez1/splitflap"
"""

_FOOTER = """
save build/panelized_sensor.kicad_pcb
"""


def run(output_file):
    with open(output_file, 'wb') as f:
        f.write(_HEADER)
        f.write('\n')

        f.write('source-area {} {} {} {}\n'.format(
            ORIGIN[0],
            ORIGIN[1],
            ORIGIN[0] + BOARD_SIZE[0],
            ORIGIN[1] + BOARD_SIZE[1],
        ))

        # Generate copies
        for r in range(ROWS):
            f.write('\n# Row {}\n'.format(r))
            for c in range(COLS):
                f.write('copy {} {}\n'.format(
                    ORIGIN[0] + c * BOARD_SIZE[0],
                    ORIGIN[1] + r * BOARD_SIZE[1],
                ))

        f.write('\n')

        # Draw board outline
        x_min = ORIGIN[0]
        x_max = ORIGIN[0] + COLS * BOARD_SIZE[0]
        y_min = ORIGIN[1]
        y_max = ORIGIN[1] + ROWS * BOARD_SIZE[1]
        draw_edge_line(f, x_min, y_min, x_max, y_min)
        draw_edge_line(f, x_max, y_min, x_max, y_max)
        draw_edge_line(f, x_max, y_max, x_min, y_max)
        draw_edge_line(f, x_min, y_max, x_min, y_min)

        # Draw v-cut lines
        for r in range(1, ROWS):
            y = ORIGIN[1] + r * BOARD_SIZE[1]
            draw_edge_line(
                f,
                x_min - VCUT_LINE_OUTSET,
                y,
                x_max + VCUT_LINE_OUTSET,
                y,
            )
            f.write('draw-text V-CUT {} {} 0 Edge.Cuts 3 2 0.25\n'.format(
                x_max + VCUT_LINE_OUTSET + VCUT_LABEL_OUTSET[0],
                y,
            ))

        for c in range(1, COLS):
            x = ORIGIN[0] + c * BOARD_SIZE[0]
            draw_edge_line(
                f,
                x,
                y_min - VCUT_LINE_OUTSET,
                x,
                y_max + VCUT_LINE_OUTSET,
            )
            f.write('draw-text V-CUT {} {} 0 Edge.Cuts 3 2 0.25\n'.format(
                x,
                y_max + VCUT_LINE_OUTSET + VCUT_LABEL_OUTSET[1],
            ))

        width = COLS * BOARD_SIZE[0]
        height = ROWS * BOARD_SIZE[1]
        f.write('\ndraw-text "{} x {}" {} {} 0 Cmts.User 4 2.5 0.3\n'.format(
            width,
            height,
            ORIGIN[0] + width/2,
            ORIGIN[1] - 5,
        ))
        f.write(_FOOTER)

    print 'Wrote {}'.format(output_file)
    print 'Board dimensions: {} x {}'.format(width, height)


def draw_edge_line(f, x1, y1, x2, y2):
    f.write('draw-line {} {} {} {} Edge.Cuts 0.15\n'.format(x1, y1, x2, y2))

if __name__ == '__main__':
    run(os.path.join('build', 'panelized_sensor_config.txt'))
