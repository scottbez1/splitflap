#!/usr/bin/env python
from __future__ import division
from __future__ import print_function

import datetime
import logging
import os
import subprocess

from svg_processor import SvgProcessor
from projection_renderer import Renderer

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    laser_parts_directory = os.path.join('build', 'laser_parts')

    git_rev = subprocess.check_output([
        'git',
        'rev-parse',
        '--short',
        'HEAD',
    ]).strip()

    extra_variables = {
        'render_revision': git_rev,
        'render_date': datetime.date.today().strftime('%Y-%m-%d'),
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

    subprocess.check_call([
        'inkscape',
        '--verb=FitCanvasToDrawing',
        '--verb=FileSave',
        '--verb=FileClose',
        raster_svg,
    ])
    subprocess.check_call([
        'inkscape',
        '--export-width=320',
        '--export-png', raster_png,
        raster_svg,
    ])

