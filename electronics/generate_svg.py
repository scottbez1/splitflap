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
import subprocess
import tempfile

from svg_processor import SvgProcessor

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

pcb_filename = 'splitflap.kicad_pcb'

# Have to use absolute path for build_directory otherwise pcbnew will output relative to the temp file
build_directory = os.path.abspath('build')

output_suffix = 'preview-vector'

def color_with_alpha(base_color, alpha):
    return (base_color & ~(0xFF << 24)) | ((alpha & 0xFF) << 24)

def get_label_attributes():
    git_rev = subprocess.check_output([
        'git',
        'rev-parse',
        '--short',
        'HEAD',
    ]).strip()

    return {
        'revision': git_rev,
        'date': datetime.date.today().strftime('%Y-%m-%d'),
    }


with open(pcb_filename, 'rb') as pcb:
    original_contents = pcb.read()

    label_attributes = get_label_attributes()
    new_contents = original_contents\
        .replace('COMMIT: deadbeef', 'COMMIT: ' + label_attributes['revision'])\
        .replace('DATE: YYYY-MM-DD', 'DATE: ' + label_attributes['date'])

with tempfile.NamedTemporaryFile(suffix='.kicad_pcb') as temp_pcb:
    logger.debug('Writing to %s', temp_pcb.name)
    temp_pcb.write(new_contents)
    temp_pcb.flush()

    logger.debug('Load board')
    board = pcbnew.LoadBoard(temp_pcb.name)

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
            'alpha': 0.5, 
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
            'alpha': 0.5, 
        },
    ]

    for i, layer in enumerate(layers):
        layer_name = 'layer-%02d' % i
        plot_controller.SetLayer(layer['layer'])
        logger.info('Plotting layer %d (kicad layer=%r)', i, layer['layer'])
        plot_controller.OpenPlotfile(layer_name, pcbnew.PLOT_FORMAT_SVG, 'Preview vector rendering')
        plot_controller.PlotLayer()
        plot_controller.ClosePlot()

        output_name = '%s-%s.svg' % (
            os.path.splitext(os.path.basename(temp_pcb.name))[0],
            layer_name,
        )

        output_filename = os.path.join(build_directory, output_name)
        output_filename2 = os.path.join(build_directory, 'processed-' + output_name)
        logger.info('Post-processing %s...', output_filename)
        processor = SvgProcessor(output_filename)
        processor.apply_color(layer['color'], layer['alpha'])
        processor.write(output_filename2)

