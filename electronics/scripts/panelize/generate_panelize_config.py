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
import argparse
import json
import os


VCUT_LINE_OUTSET = 2
VCUT_LABEL_OUTSET = (6, 3)


class Metaconfig(object):
    def __init__(self, json_data):
        self.pcb_title = json_data['pcb_title']
        self.pcb_comment = json_data['pcb_comment']
        self.origin = json_data['origin']
        self.board_size = json_data['board_size']
        self.rows = json_data['rows']
        self.cols = json_data['cols']
        self.input_kicad_pcb = json_data['input_kicad_pcb']
        self.output_kicad_pcb = json_data['output_kicad_pcb']
        self.output_panelize_config = json_data['output_panelize_config']


def _load_metaconfig(filepath):
    with open(filepath, 'rb') as f:
        json_data = json.load(f)
    return Metaconfig(json_data)


def run(metaconfig_filepath):
    config = _load_metaconfig(metaconfig_filepath)

    header = """
load "{input_pcb}"
create-template
set-title "{title}"
set-comment-1 "{comment}"
    """.format(input_pcb=config.input_kicad_pcb, title=config.pcb_title, comment=config.pcb_comment)

    footer = """
save "{output_pcb}"
    """.format(output_pcb=config.output_kicad_pcb)

    with open(config.output_panelize_config, 'wb') as f:
        f.write(header)
        f.write('\n')

        f.write('source-area {} {} {} {}\n'.format(
            config.origin[0],
            config.origin[1],
            config.origin[0] + config.board_size[0],
            config.origin[1] + config.board_size[1],
        ))

        f.write('exclude-layer Edge.Cuts\n')

        # Generate copies
        for r in range(config.rows):
            f.write('\n# Row {}\n'.format(r))
            for c in range(config.cols):
                f.write('copy {} {}\n'.format(
                    config.origin[0] + c * config.board_size[0],
                    config.origin[1] + r * config.board_size[1],
                ))

        f.write('\n')

        # Draw board outline
        f.write('\n# Borders\n')
        x_min = config.origin[0]
        x_max = config.origin[0] + config.cols * config.board_size[0]
        y_min = config.origin[1]
        y_max = config.origin[1] + config.rows * config.board_size[1]
        draw_edge_line(f, x_min, y_min, x_max, y_min)
        draw_edge_line(f, x_max, y_min, x_max, y_max)
        draw_edge_line(f, x_max, y_max, x_min, y_max)
        draw_edge_line(f, x_min, y_max, x_min, y_min)

        # Draw v-cut lines
        f.write('\n# V-CUT lines and labels\n')
        for r in range(1, config.rows):
            y = config.origin[1] + r * config.board_size[1]
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

        for c in range(1, config.cols):
            x = config.origin[0] + c * config.board_size[0]
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

        f.write('\n# Overall board dimensions\n')
        width = config.cols * config.board_size[0]
        height = config.rows * config.board_size[1]
        f.write('\ndraw-text "{} x {}" {} {} 0 Cmts.User 4 2.5 0.3\n'.format(
            width,
            height,
            config.origin[0] + width/2,
            config.origin[1] - 5,
        ))
        f.write(footer)

    print 'Wrote {}'.format(f.name)
    print 'Board dimensions: {} x {}'.format(width, height)


def draw_edge_line(f, x1, y1, x2, y2):
    f.write('draw-line {} {} {} {} Edge.Cuts 0.15\n'.format(x1, y1, x2, y2))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generates panelize config given a metaconfig file')
    parser.add_argument('metaconfig_file', help='metaconfig json file')
    args = parser.parse_args()
    run(args.metaconfig_file)
