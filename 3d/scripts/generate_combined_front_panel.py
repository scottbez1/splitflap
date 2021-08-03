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

CENTER_MODES = {
    'letter': 0,
    'window': 1,
    'module': 2,
}

def render(extra_variables, output_directory):
    renderer = Renderer(os.path.join(source_parts_dir, 'combined_front_panel.scad'), output_directory, extra_variables)
    renderer.clean()
    svg_output = renderer.render_svgs(panelize_quantity = 1)
    logging.info('\n\n\nDone rendering to SVG: ' + svg_output)

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser("")

    kerf_group = parser.add_mutually_exclusive_group(required=True)
    kerf_group.add_argument('--kerf', type=float, help='Set kerf_width value')
    kerf_group.add_argument('--kerf-preset', choices=KERF_PRESETS, help='Set kerf_width using a defined preset')
    kerf_group.add_argument('--tool-diameter', type=float, help='Diameter of cutting tool')

    parser.add_argument('--rows', type=int, required=True, help='Number of rows')
    parser.add_argument('--cols', type=int, required=True, help='Number of columns')

    x_group = parser.add_mutually_exclusive_group(required=True)
    x_group.add_argument('--spacing-x', type=float, help='Horizontal gap between modules')
    x_group.add_argument('--center-center-x', type=float, help='Horizontal center-to-center distance between modules')

    y_group = parser.add_mutually_exclusive_group(required=True)
    y_group.add_argument('--spacing-y', type=float, help='Vertical gap between modules')
    y_group.add_argument('--center-center-y', type=float, help='Vertical center-to-center distance between modules')

    width_group = parser.add_mutually_exclusive_group(required=True)
    width_group.add_argument('--width', type=float, help='Width of the panel')
    width_group.add_argument('--frame-margin-x', type=float, help='Margin to add to the left and right sides')

    height_group = parser.add_mutually_exclusive_group(required=True)
    height_group.add_argument('--height', type=float, help='Height of the panel')
    height_group.add_argument('--frame-margin-y', type=float, help='Margin to add to the top and bottom')

    parser.add_argument('--center-mode', choices=CENTER_MODES, required=True, help='Specify how modules should be centered')

    args = parser.parse_args()

    extra_variables = {
        'render_etch': False,
    }
    if args.kerf is not None:
        extra_variables['kerf_width'] = args.kerf
    elif args.kerf_preset is not None:
        extra_variables['kerf_width'] = KERF_PRESETS[args.kerf_preset]
    elif args.tool_diameter is not None:
        extra_variables['tool_diameter'] = args.tool_diameter

    extra_variables['rows'] = args.rows
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

    if args.frame_margin_x is not None:
        extra_variables['frame_margin_x'] = args.frame_margin_x
    if args.frame_margin_y is not None:
        extra_variables['frame_margin_y'] = args.frame_margin_y

    extra_variables['center_mode'] = CENTER_MODES[args.center_mode]

    output_dir = os.path.join(source_parts_dir, 'build', 'front_panel')

    render(extra_variables, output_dir)
