#!/usr/bin/env python

#   Copyright 2015-2020 Scott Bezek and the splitflap contributors
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
repo_root = os.path.dirname(script_dir)
sys.path.append(repo_root)

from util import rev_info

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser()
    parser.add_argument('--panelize', type=int, default=1, help='Quantity to panelize - must be 1 or an even number')
    parser.add_argument('--skip-optimize', action='store_true', help='Don\'t remove redundant/overlapping cut lines')
    parser.add_argument('--kerf', type=float, help='Override kerf_width value')
    parser.add_argument('--render-raster', action='store_true', help='Render raster PNG from the output SVG (requires '
                                                                     'Inkscape)')
    parser.add_argument('--thickness', type=float, help='Override panel thickness value')

    args = parser.parse_args()

    laser_parts_directory = os.path.join(script_dir, 'build', 'laser_parts')

    extra_variables = {
        'render_revision': rev_info.git_short_rev(),
        'render_date': rev_info.current_date(),
    }
    if args.kerf is not None:
        extra_variables['kerf_width'] = args.kerf
    if args.thickness is not None:
        extra_variables['thickness'] = args.thickness

    renderer = Renderer(os.path.join(script_dir, 'splitflap.scad'), laser_parts_directory, extra_variables)
    renderer.clean()
    svg_output = renderer.render_svgs(panelize_quantity=args.panelize)

    logging.info('Removing redundant lines')
    processor = SvgProcessor(svg_output)

    redundant_lines, merged_lines = None, None
    if not args.skip_optimize:
        redundant_lines, merged_lines = processor.remove_redundant_lines()

    processor.write(svg_output)

    logging.info('\n\n\nDone rendering to SVG: ' + svg_output)

    if args.render_raster:
        # Export to png
        logging.info('Generating raster preview')
        raster_svg = os.path.join(laser_parts_directory, 'raster.svg')
        raster_png = os.path.join(laser_parts_directory, 'raster.png')
        processor.apply_raster_render_style()

        if not args.skip_optimize:
            # Show which redundant lines were removed and lines merged
            processor.add_highlight_lines(redundant_lines, '#ff0000')
            processor.add_highlight_lines(merged_lines, '#0000ff')

        processor.write(raster_svg)

        logging.info('Resize SVG canvas')
        subprocess.check_call([
            'inkscape',
            '--verb=FitCanvasToDrawing',
            '--verb=FileSave',
            '--verb=FileClose',
            raster_svg,
        ])
        logging.info('Export PNG')
        subprocess.check_call([
            'inkscape',
            '--export-width=320',
            '--export-png', raster_png,
            raster_svg,
        ])

