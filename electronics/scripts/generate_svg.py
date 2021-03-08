#!/usr/bin/env python3
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
import argparse
import logging
import os
import pcbnew
import shutil
import subprocess

import pcb_util

from svg_processor import SvgProcessor

electronics_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)


def color_with_alpha(base_color, alpha):
    return (base_color & ~(0xFF << 24)) | ((alpha & 0xFF) << 24)


def run(pcb_file):
    output_directory = os.path.join(electronics_root, 'build')
    temp_dir = os.path.join(output_directory, 'temp_layers')
    shutil.rmtree(temp_dir, ignore_errors=True)
    try:
        os.makedirs(temp_dir)
        plot_to_directory(pcb_file, output_directory, temp_dir)
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)


def plot_to_directory(pcb_file, output_directory, temp_dir):
    board_name = os.path.splitext(os.path.basename(pcb_file))[0]

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
        {
            'layer': pcbnew.Cmts_User,
            'color': '#333333',
            'alpha': 0.8,
        },
        {
            'layer': pcbnew.Edge_Cuts,
            'color': '#3333CC',
            'alpha': 0.8,
        },
    ]
    with pcb_util.get_plotter(pcb_file, temp_dir) as plotter:
        plotter.plot_options.SetExcludeEdgeLayer(True)
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

        # Plot the paste layer to its own SVG
        logger.info('Plotting paste SVG')
        output_filename = plotter.plot(pcbnew.F_Paste, pcbnew.PLOT_FORMAT_SVG)
        processor = SvgProcessor(output_filename)
        def colorize(original):
            if original.lower() == '#000000':
                return '#FF0000'
            return original
        processor.apply_group_style_transforms({
            'fill-opacity': lambda _: '0',
            'stroke': lambda _: '#FF0000',
            'stroke-opacity': lambda _: '1',
            'stroke-width': lambda _: '20',
        })
        paste_filename = os.path.join(output_directory, '%s_paste.svg' % board_name)
        processor.write(paste_filename)


        logger.info('Merging layers...')
        final_svg = os.path.join(output_directory, '%s_merged.svg' % board_name)

        shutil.copyfile(processed_svg_files[0][0], final_svg)
        output_processor = SvgProcessor(final_svg)
        for _, processor in processed_svg_files:
            output_processor.import_groups(processor)
        output_processor.write(final_svg)

        logger.info('Rasterizing...')
        raster_width = 1280
        final_png = os.path.join(output_directory, '%s_merged.png' % board_name)
        subprocess.check_call([
            'inkscape',
            '--export-area-drawing',
            '--export-width', str(raster_width),
            '--export-png', final_png,
            '--export-background', '#FFFFFF',
            final_svg,
        ])

if __name__ == '__main__':
    parser = argparse.ArgumentParser('Generate an SVG rendering of the PCB')
    parser.add_argument('pcb_file')
    args = parser.parse_args()
    run(args.pcb_file)
