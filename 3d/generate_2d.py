#!/usr/bin/env python

#   Copyright 2015 Scott Bezek
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

import logging
import os
import subprocess
import sys

from svg_processor import SvgProcessor
from projection_renderer import Renderer

repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(repo_root)

from util import rev_info

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    laser_parts_directory = os.path.join('build', 'laser_parts')

    extra_variables = {
        'render_revision': rev_info.git_short_rev(),
        'render_date': rev_info.current_date(),
    }

    renderer = Renderer('splitflap.scad', laser_parts_directory, extra_variables)
    renderer.clean()
    svg_output = renderer.render_svgs()

    # Export to png
    raster_svg = os.path.join(laser_parts_directory, 'raster.svg')
    raster_png = os.path.join(laser_parts_directory, 'raster.png')
    processor = SvgProcessor(svg_output)
    processor.apply_raster_render_style()
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

