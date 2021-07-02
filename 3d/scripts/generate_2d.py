#!/usr/bin/env python3

#   Copyright 2015-2021 Scott Bezek and the splitflap contributors
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
import logging
import os
import subprocess
import sys
import zipfile

from svg_processor import SvgProcessor
from projection_renderer import Renderer

script_dir = os.path.dirname(os.path.abspath(__file__))
source_parts_dir = os.path.dirname(script_dir)
repo_root = os.path.dirname(source_parts_dir)
sys.path.append(repo_root)

from util import rev_info

KERF_PRESETS = {
    'ponoko-3mm-mdf': 0.18,
    'ponoko-3mm-acrylic': 0.1,
    'elecrow-3mm-wood': 0.185,
}

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser()
    parser.add_argument('--panelize', type=int, default=1, help='Quantity to panelize - must be 1 or an even number')
    parser.add_argument('--skip-optimize', action='store_true', help='Don\'t remove redundant/overlapping cut lines')

    kerf_group = parser.add_mutually_exclusive_group()
    kerf_group.add_argument('--kerf', type=float, help='Override kerf_width value')
    kerf_group.add_argument('--kerf-preset', choices=KERF_PRESETS, help='Override kerf_width using a defined preset')

    parser.add_argument('--render-raster', action='store_true', help='Render raster PNG from the output SVG (requires '
                                                                     'Inkscape)')
    parser.add_argument('--calculate-dimensions', action='store_true', help='Calculate overall cut file dimensions ('
                                                                            'requires Inkscape)')
    parser.add_argument('--thickness', type=float, help='Override panel thickness value')
    parser.add_argument('--no-etch', action='store_true', help='Do not render laser-etched features')
    parser.add_argument('--mirror', action='store_true', help='Mirror the assembly so the outside faces are facing up. '
                                                              'Note that this will remove all etched features.')
    parser.add_argument('--render-elecrow', action='store_true', help='Render an additional zipped pdf with labeled '
                                                                      'dimensions and only cut lines, for Elecrow. '
                                                                      'Requires Inkscape and pdfjam. Implies '
                                                                      '--no-etch, --calculate-dimensions, and '
                                                                      '--skip-optimize')

    args = parser.parse_args()
    if args.render_elecrow:
        args.no_etch = True
        args.calculate_dimensions = True
        args.skip_optimize = True

    laser_parts_directory = os.path.join(source_parts_dir, 'build', 'laser_parts')

    extra_variables = {
        'render_revision': rev_info.git_short_rev(),
        'render_date': rev_info.current_date(),
        'render_etch': not args.no_etch,
        'render_2d_mirror': args.mirror,
    }
    if args.kerf is not None:
        extra_variables['kerf_width'] = args.kerf
    elif args.kerf_preset is not None:
        extra_variables['kerf_width'] = KERF_PRESETS[args.kerf_preset]
    if args.thickness is not None:
        extra_variables['thickness'] = args.thickness

    print('Variables:\n' + json.dumps(extra_variables, indent=4))

    renderer = Renderer(os.path.join(source_parts_dir, 'splitflap.scad'), laser_parts_directory, extra_variables)
    renderer.clean()
    svg_output = renderer.render_svgs(panelize_quantity=args.panelize)

    processor = SvgProcessor(svg_output)

    redundant_lines, merged_lines = None, None
    if not args.skip_optimize:
        logging.info('Removing redundant lines')
        redundant_lines, merged_lines = processor.remove_redundant_lines()

    processor.write(svg_output)
    logging.info(f'\n\n\nDone rendering to SVG: {svg_output}')

    if args.calculate_dimensions:
        svg_for_dimensions = os.path.join(laser_parts_directory, os.path.splitext(os.path.basename(svg_output))[0] + '_dimensions.svg')
        processor.apply_dimension_calculation_style()
        processor.write(svg_for_dimensions)
        logging.info('Read dimensions')
        width_px = float(subprocess.check_output([
            'inkscape',
            '--without-gui',
            '--query-width',
            svg_for_dimensions,
        ]))
        width_mm = width_px * 25.4 / 96 # Assuming 96dpi...
        height_px = float(subprocess.check_output([
            'inkscape',
            '--without-gui',
            '--query-height',
            svg_for_dimensions,
        ]))
        height_mm = height_px * 25.4 / 96 # Assuming 96dpi...
        dimensions_file = os.path.join(laser_parts_directory, os.path.splitext(os.path.basename(svg_output))[0] + '_dimensions.txt')
        with open(dimensions_file, 'w') as f:
            f.write(f'{width_mm:.2f}mm x {height_mm:.2f}mm')

    if args.render_elecrow:
        elecrow_svg = os.path.join(laser_parts_directory, 'elecrow.svg')
        elecrow_intermediate_pdf = os.path.join(laser_parts_directory, 'elecrow_intermediate.pdf')
        elecrow_pdf = os.path.join(laser_parts_directory, 'elecrow.pdf')
        elecrow_zip = os.path.join(laser_parts_directory, 'elecrow.zip')

        processor.apply_elecrow_style()
        processor.add_dimensions(width_mm, height_mm)
        processor.write(elecrow_svg)

        logging.info('Resize SVG canvas')
        subprocess.check_call([
            'inkscape',
            '--verb=FitCanvasToDrawing',
            '--verb=FileSave',
            '--verb=FileClose',
            '--verb=FileQuit',
            elecrow_svg,
        ])

        logging.info('Output PDF')
        subprocess.check_call([
            'inkscape',
            elecrow_svg,
            '--export-pdf',
            elecrow_intermediate_pdf,
        ])

        logging.info('Resize PDF')
        subprocess.check_call([
            'pdfjam',
            '--letterpaper',
            '--landscape',
            '--noautoscale',
            'true',
            '--outfile',
            elecrow_pdf,
            elecrow_intermediate_pdf,
        ])

        logging.info('Zip')
        with zipfile.ZipFile(elecrow_zip, 'w', zipfile.ZIP_DEFLATED) as zip:
            zip.write(elecrow_pdf, 'elecrow.pdf')

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
            '--verb=FileQuit',
            raster_svg,
        ])

        logging.info('Export PNG')
        subprocess.check_call([
            'inkscape',
            '--export-width=320',
            '--export-png', raster_png,
            raster_svg,
        ])

