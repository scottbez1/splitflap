#!/usr/bin/env python3

#   Copyright 2020 Scott Bezek and the splitflap contributors
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


from __future__ import division
from __future__ import print_function

import argparse
import logging
import os
import subprocess
import sys

from svg_processor import SvgProcessor
from projection_renderer import Renderer

script_dir = os.path.dirname(os.path.abspath(__file__))
source_parts_dir = os.path.dirname(script_dir)
repo_root = os.path.dirname(source_parts_dir)
sys.path.append(repo_root)

from util import rev_info

_MODE_DOUBLE_SIDED = 'double-sided'
_MODE_FULL_FONT = 'full-font'

def render(extra_variables, skip_optimize, output_directory):
    renderer = Renderer(os.path.join(source_parts_dir, 'font_generator.scad'), output_directory, extra_variables)
    renderer.clean()
    svg_output = renderer.render_svgs(panelize_quantity = 1)

    processor = SvgProcessor(svg_output)

    redundant_lines, merged_lines = None, None
    if not skip_optimize:
        logging.info('Removing redundant lines')
        redundant_lines, merged_lines = processor.remove_redundant_lines()

    processor.write(svg_output)
    logging.info('\n\n\nDone rendering to SVG: ' + svg_output)

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser(add_help=False)  # avoid conflict with '-h' for height

    parser.add_argument('--mode', choices=[_MODE_DOUBLE_SIDED, _MODE_FULL_FONT])

    parser.add_argument('-f', '--font', type=str, help='Name of font to use (no spaces)')
    parser.add_argument('-t', '--text', type=str, help='String of text to generate')

    parser.add_argument('--columns', type=int, help='Number of columns')
    parser.add_argument('--start-row', type=int, help='The starting row to render, 0-indexed')
    parser.add_argument('--row-count', type=int, help='Number of rows to render')

    parser.add_argument('--spacing-x', type=float, help='Horizontal gap between flaps')
    parser.add_argument('--spacing-y', type=float, help='Vertical gap between flaps')

    parser.add_argument('--no-comp', action='store_true', default=False, help='Don\'t compensate for the gap between top and bottom flaps')
    parser.add_argument('--alignment', action='store_true', help='Render alignment markers for registration when printing onto flaps')
    parser.add_argument('--flip', action='store_true', help='Flip the design over')

    parser.add_argument('--bleed', type=float, help='Bleed amount for letters, in mm')
    parser.add_argument('--kerf', type=float, help='Override kerf_width value')
    parser.add_argument('--fill', action='store_true', help='Fill the text solid (disables optimization)')
    parser.add_argument('--skip-optimize', action='store_true', help='Don\'t remove redundant/overlapping cut lines')

    parser.add_argument("--help", action="help", help="show this help message and exit")

    args = parser.parse_args()

    extra_variables = {
        'render_revision': rev_info.git_short_rev(),
        'render_date': rev_info.current_date(),
        'letter_gap_comp' : not args.no_comp,
    }
    if args.font is not None:
        extra_variables['font_preset'] = args.font
    if args.text is not None:
        extra_variables['character_list'] = args.text

    if args.start_row is not None:
        extra_variables['start_row'] = args.start_row
    if args.row_count is not None:
        extra_variables['row_count'] = args.row_count
    if args.columns is not None:
        extra_variables['num_columns'] = args.columns

    if args.spacing_x is not None:
        extra_variables['spacing_x'] = args.spacing_x
    if args.spacing_y is not None:
        extra_variables['spacing_y'] = args.spacing_y

    if args.kerf is not None:
        extra_variables['kerf_width'] = args.kerf
    if args.fill:
        extra_variables['render_fill'] = True
        args.skip_optimize = True
    if args.bleed is not None:
        extra_variables['bleed'] = args.bleed

    fonts_directory = os.path.join(source_parts_dir, 'build', 'fonts')

    if args.mode == _MODE_DOUBLE_SIDED:
        extra_variables['layout_double_flaps_for_full_font'] = False
        extra_variables['render_alignment_marks'] = args.alignment

        extra_variables['only_side'] = 1
        extra_variables['flip_over'] = args.flip
        render(extra_variables, args.skip_optimize, os.path.join(fonts_directory, 'front'))

        extra_variables['only_side'] = 2
        extra_variables['flip_over'] = not args.flip
        render(extra_variables, args.skip_optimize, os.path.join(fonts_directory, 'back'))
    else:
        if args.alignment:
            raise RuntimeError('Alignment markers are incompatible with full font rendering')
        extra_variables['layout_double_flaps_for_full_font'] = True
        extra_variables['only_side'] = 1
        extra_variables['flip_over'] = args.flip
        render(extra_variables, args.skip_optimize, fonts_directory)
