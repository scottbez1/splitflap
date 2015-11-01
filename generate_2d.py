#!/usr/bin/env python
from __future__ import division
from __future__ import print_function

import logging
import os
import subprocess

from svg_processor import SvgProcessor
from projection_renderer import Renderer

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    laser_parts_directory = os.path.join('build', 'laser_parts')

    renderer = Renderer('splitflap.scad', laser_parts_directory)
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

