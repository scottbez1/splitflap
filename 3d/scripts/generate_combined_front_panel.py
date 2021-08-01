#!/usr/bin/env python3

#   Copyright 2021 Scott Bezek and the splitflap contributors
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
import sys

from kerf_presets import KERF_PRESETS
from svg_processor import SvgProcessor
from projection_renderer import Renderer

script_dir = os.path.dirname(os.path.abspath(__file__))
source_parts_dir = os.path.dirname(script_dir)
repo_root = os.path.dirname(source_parts_dir)
sys.path.append(repo_root)

def render(extra_variables, output_directory):
    renderer = Renderer(os.path.join(source_parts_dir, 'combined_front_panel.scad'), output_directory, extra_variables)
    renderer.clean()
    svg_output = renderer.render_svgs(panelize_quantity = 1)
    logging.info('\n\n\nDone rendering to SVG: ' + svg_output)

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser("")

    kerf_group = parser.add_mutually_exclusive_group()
    kerf_group.add_argument('--kerf', type=float, help='Override kerf_width value')
    kerf_group.add_argument('--kerf-preset', choices=KERF_PRESETS, help='Override kerf_width using a defined preset')

    parser.add_argument('--rows', type=int, help='Number of rows')
    parser.add_argument('--cols', type=int, help='Number of columns')

    x_group = parser.add_mutually_exclusive_group()
    x_group.add_argument('--spacing-x', type=float, help='Horizontal gap between modules')
    x_group.add_argument('--center-center-x', type=float, help='Horizontal center-to-center distance between modules')

    y_group = parser.add_mutually_exclusive_group()
    y_group.add_argument('--spacing-y', type=float, help='Vertical gap between modules')
    y_group.add_argument('--center-center-y', type=float, help='Vertical center-to-center distance between modules')

    parser.add_argument('--width', type=float, help='Width of the panel')
    parser.add_argument('--height', type=float, help='Height of the panel')

    parser.add_argument('--tool-diameter', type=float, help='Diameter of cutting tool')
    parser.add_argument('--center-window', action='store_true', help='Vertically center the window instead of the '
                                                                     'default of vertically centering the front '
                                                                     'flap/letter.')

    args = parser.parse_args()

    extra_variables = {
        'render_etch': False,
    }
    if args.kerf is not None:
        extra_variables['kerf_width'] = args.kerf
    elif args.kerf_preset is not None:
        extra_variables['kerf_width'] = KERF_PRESETS[args.kerf_preset]

    if args.rows is not None:
        extra_variables['rows'] = args.rows
    if args.cols is not None:
        extra_variables['cols'] = args.cols

    if args.spacing_x is not None:
        extra_variables['gap_x'] = args.spacing_x
    if args.spacing_y is not None:
        extra_variables['gap_y'] = args.spacing_y
    if args.center_center_x is not None:
        extra_variables['center_center_x'] = args.center_center_x
    if args.center_center_y is not None:
        extra_variables['center_center_y'] = args.center_center_y

    if args.width is not None:
        extra_variables['frame_width'] = args.width
    if args.height is not None:
        extra_variables['frame_height'] = args.height

    if args.tool_diameter is not None:
        extra_variables['tool_diameter'] = args.tool_diameter

    extra_variables['center_window'] = args.center_window

    output_dir = os.path.join(source_parts_dir, 'build', 'front_panel')

    render(extra_variables, output_dir)
