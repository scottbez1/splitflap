#!/usr/bin/env python
#   Copyright 2015-2016 Scott Bezek and the splitflap contributors
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

import logging
import os
import pcbnew
import shutil
import subprocess

import pcb_util

from svg_processor import SvgProcessor

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

PCB_FILENAME = 'splitflap.kicad_pcb'

# Have to use absolute path for build_directory otherwise pcbnew will output relative to the temp file
BUILD_DIRECTORY = os.path.abspath('build')

def color_with_alpha(base_color, alpha):
    return (base_color & ~(0xFF << 24)) | ((alpha & 0xFF) << 24)

def run():
    temp_dir = os.path.join(BUILD_DIRECTORY, 'temp_layers')
    shutil.rmtree(temp_dir, ignore_errors=True)
    try:
        os.makedirs(temp_dir)
        plot_to_directory(BUILD_DIRECTORY, temp_dir)
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)

def plot_to_directory(output_directory, temp_dir):
    layers = [
        {
            'layer': pcbnew.B_SilkS,
            'color': '#CC00CC',
            'alpha': 0.8,
        },
        {
            'layer': pcbnew.B_Cu,
            'color': '#33EE33',
            'alpha': 0.5,
        },
        {
            'layer': pcbnew.F_Cu,
            'color': '#CC0000',
            'alpha': 0.5,
        },
        {
            'layer': pcbnew.F_SilkS,
            'color': '#00CCCC',
            'alpha': 0.8,
        },
    ]
    with pcb_util.get_plotter(PCB_FILENAME, temp_dir) as plotter:
        plotter.plot_options.SetExcludeEdgeLayer(False)
        processed_svg_files = []
        for i, layer in enumerate(layers):
            output_filename = plotter.plot(layer['layer'], pcbnew.PLOT_FORMAT_SVG)
            logger.info('Post-processing %s...', output_filename)
            processor = SvgProcessor(output_filename)
            def colorize(original):
                if original.lower() == '#000000':
                    return layer['color']
                return original
            processor.apply_color_transform(colorize)
            processor.wrap_with_group({
                'opacity': str(layer['alpha']),
            })

            output_filename2 = os.path.join(temp_dir, 'processed-' + os.path.basename(output_filename))
            processor.write(output_filename2)
            processed_svg_files.append((output_filename2, processor))

        logger.info('Merging layers...')
        final_svg = os.path.join(output_directory, 'merged.svg')

        shutil.copyfile(processed_svg_files[0][0], final_svg)
        output_processor = SvgProcessor(final_svg)
        for _, processor in processed_svg_files:
            output_processor.import_groups(processor)
        output_processor.write(final_svg)

        logger.info('Rasterizing...')
        raster_width = 1280
        final_png = os.path.join(output_directory, 'merged.png')
        subprocess.check_call([
            'inkscape',
            '--export-area-drawing',
            '--export-width', str(raster_width),
            '--export-png', final_png,
            '--export-background', '#FFFFFF',
            final_svg,
        ])

if __name__ == '__main__':
    run()
