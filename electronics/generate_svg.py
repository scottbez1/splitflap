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

import datetime
import logging
import os
import pcbnew
import re
import shutil
import subprocess
import tempfile
import traceback

import pcb_util

from svg_processor import SvgProcessor

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

pcb_filename = 'splitflap.kicad_pcb'

# Have to use absolute path for build_directory otherwise pcbnew will output relative to the temp file
build_directory = os.path.abspath('build')

def color_with_alpha(base_color, alpha):
    return (base_color & ~(0xFF << 24)) | ((alpha & 0xFF) << 24)

with pcb_util.versioned_board(pcb_filename) as board:
    plot_controller = pcbnew.PLOT_CONTROLLER(board)
    plot_options = plot_controller.GetPlotOptions()

    plot_options.SetOutputDirectory(build_directory)

    plot_options.SetPlotFrameRef(False)
    plot_options.SetLineWidth(pcbnew.FromMM(0.35))

    plot_options.SetScale(1)
    plot_options.SetUseAuxOrigin(True)
    plot_options.SetMirror(False)
    plot_options.SetExcludeEdgeLayer(False)

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

    files_to_cleanup = set()

    try:
        processed_svg_files = []
        for i, layer in enumerate(layers):
            layer_name = 'layer-%02d' % i
            plot_controller.SetLayer(layer['layer'])
            logger.info('Plotting layer %d (kicad layer=%r)', i, layer['layer'])

            output_name = '%s-%s.svg' % (
                os.path.splitext(os.path.basename(board.GetFileName()))[0],
                layer_name,
            )

            output_filename = os.path.join(build_directory, output_name)
            output_filename2 = os.path.join(build_directory, 'processed-' + output_name)

            files_to_cleanup.add(output_filename)
            plot_controller.OpenPlotfile(layer_name, pcbnew.PLOT_FORMAT_SVG, 'Preview vector rendering')
            plot_controller.PlotLayer()
            plot_controller.ClosePlot()

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
            files_to_cleanup.add(output_filename2)
            processor.write(output_filename2)
            processed_svg_files.append((output_filename2, processor))

        logger.info('Merging layers...')
        final_svg = os.path.join(build_directory, 'merged.svg')

        files_to_cleanup.add(final_svg)
        shutil.copyfile(processed_svg_files[0][0], final_svg)
        output_processor = SvgProcessor(final_svg)
        for _, processor in processed_svg_files:
            output_processor.import_groups(processor)
        output_processor.write(final_svg)

        logger.info('Rasterizing...')
        final_png = os.path.join(build_directory, 'merged.png')

        files_to_cleanup.add(final_png)
        subprocess.check_call([
            'inkscape',
            '--export-area-drawing',
            '--export-width=320',
            '--export-png', final_png,
            '--export-background', '#FFFFFF',
            final_svg,
        ])

        # Completed successfully, so don't clean up the final artifacts
        files_to_cleanup.remove(final_svg)
        files_to_cleanup.remove(final_png)
    except:
        logger.critical(traceback.format_exc())
        leave_intermediates = raw_input('Build failed. Leave intermediate files around? [Yn]')
        leave_intermediates = leave_intermediates == '' or leave_intermediates.lower()[0] != 'n'
        if leave_intermediates:
            files_to_cleanup.clear()
        raise
    finally:
        for f in files_to_cleanup:
            logger.debug('Deleting intermediate file %r', f)
            try:
                os.remove(f)
            except:
                logger.warn('Failed to delete intermediate file %r', f)

