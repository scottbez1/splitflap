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

from collections import namedtuple

import pcb_util

from svg_processor import SvgProcessor

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

pcb_filename = 'splitflap.kicad_pcb'

# Have to use absolute path for build_directory otherwise pcbnew will output relative to the temp file
build_directory = os.path.abspath('build')

LayerDef = namedtuple('PlotDef', ['layer', 'extension'])

with pcb_util.versioned_board(pcb_filename) as board:
    plot_controller = pcbnew.PLOT_CONTROLLER(board)
    plot_options = plot_controller.GetPlotOptions()

    plot_options.SetOutputDirectory(build_directory)

    plot_options.SetPlotFrameRef(False)
    plot_options.SetLineWidth(pcbnew.FromMM(0.35))

    plot_options.SetScale(1)
    plot_options.SetUseAuxOrigin(True)
    plot_options.SetMirror(False)
    plot_options.SetExcludeEdgeLayer(True)
    plot_options.SetDrillMarksType(pcbnew.PCB_PLOT_PARAMS.NO_DRILL_SHAPE)

    layers = [
        LayerDef(pcbnew.F_Cu,       'GTL'),
        LayerDef(pcbnew.F_Mask,     'GTS'),
        LayerDef(pcbnew.F_SilkS,    'GTO'),
        LayerDef(pcbnew.B_Cu,       'GBL'),
        LayerDef(pcbnew.B_Mask,     'GBS'),
        LayerDef(pcbnew.B_SilkS,    'GBO'),
        LayerDef(pcbnew.Edge_Cuts,  'GML'),
    ]

    processed_svg_files = []
    for layer in layers:
        plot_controller.SetLayer(layer.layer)
        logger.info('Plotting layer %s (kicad layer=%r)', pcb_util.get_layer_name(layer.layer), layer.layer)

        layer_name = pcb_util.get_layer_name(layer.layer)

        plot_controller.OpenPlotfile(layer_name, pcbnew.PLOT_FORMAT_GERBER, 'Plot')
        plot_controller.PlotLayer()
        plot_controller.ClosePlot()

        output_name = '%s-%s.gbr' % (
            os.path.splitext(os.path.basename(board.GetFileName()))[0],
            layer_name,
        )

        output_filename = os.path.join(build_directory, output_name)

        desired_name = '%s.%s' % (
            os.path.splitext(os.path.basename(pcb_filename))[0],
            layer.extension,
        )
        desired_output_filename = os.path.join(build_directory, desired_name)

        os.rename(output_filename, desired_output_filename)



