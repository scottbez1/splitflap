#!/usr/bin/env python

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

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser(add_help=False)  # avoid conflict with '-h' for height
    parser.add_argument('-f', '--font', type=str, help='Name of font to use (no spaces)')
    parser.add_argument('-t', '--text', type=str, help='String of text to generate')

    parser.add_argument('-h', '--height', type=float, help='Font height, as scalar of total flap height')
    parser.add_argument('-w', '--width', type=float, help='Font width, as scalar of total flap width')
    parser.add_argument('-x', '--offset-x', type=float, help='Character offset from center, X-axis')
    parser.add_argument('-y', '--offset-y', type=float, help='Character offset from center, Y-axis')

    parser.add_argument('-nc', '--num-columns', type=int, help='Number of characters per row')
    parser.add_argument('-gc', '--gap-comp', action='store_true', help='Compensate for the gap between top and bottom flaps')

    parser.add_argument('--kerf', type=float, help='Override kerf_width value')
    parser.add_argument('--skip-optimize', action='store_true', help='Don\'t remove redundant/overlapping cut lines')

    parser.add_argument("--help", action="help", help="show this help message and exit")

    args = parser.parse_args()

    output_directory = os.path.join(source_parts_dir, 'build', 'fonts')

    extra_variables = {
        'render_revision': rev_info.git_short_rev(),
        'render_date': rev_info.current_date(),
    }
    if args.font is not None:
        extra_variables['letter_font'] = args.font
    if args.text is not None:
        extra_variables['character_list'] = args.text

    if args.height is not None:
        extra_variables['letter_height'] = args.height
    if args.width is not None:
        extra_variables['letter_width'] = args.width
    if args.offset_x is not None:
        extra_variables['letter_offset_x'] = args.offset_x
    if args.offset_y is not None:
        extra_variables['letter_offset_y'] = args.offset_y

    if args.num_columns is not None:
        extra_variables['num_columns'] = args.num_columns
    if args.gap_comp is True:
        extra_variables['letter_gap_comp'] = args.gap_comp

    if args.kerf is not None:
        extra_variables['kerf_width'] = args.kerf

    renderer = Renderer(os.path.join(source_parts_dir, 'font_generator.scad'), output_directory, extra_variables)
    renderer.clean()
    svg_output = renderer.render_svgs(panelize_quantity = 1)

    processor = SvgProcessor(svg_output)

    redundant_lines, merged_lines = None, None
    if not args.skip_optimize:
        logging.info('Removing redundant lines')
        redundant_lines, merged_lines = processor.remove_redundant_lines()

    processor.write(svg_output)

    logging.info('\n\n\nDone rendering to SVG: ' + svg_output)
