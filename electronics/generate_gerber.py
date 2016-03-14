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
from contextlib import contextmanager

import pcb_util

from svg_processor import SvgProcessor

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

PCB_FILENAME = 'splitflap.kicad_pcb'

# Have to use absolute path for build_directory otherwise pcbnew will output relative to the temp file
BUILD_DIRECTORY = os.path.abspath('build')

def run():
    temp_dir = os.path.join(BUILD_DIRECTORY, 'temp_gerbers')
    shutil.rmtree(temp_dir, ignore_errors=True)
    try:
        plot_to_directory(BUILD_DIRECTORY, temp_dir)
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)

def plot_to_directory(output_directory, temp_dir):
    board_name = os.path.splitext(os.path.basename(PCB_FILENAME))[0]

    LayerDef = namedtuple('PlotDef', ['layer', 'extension'])
    with pcb_util.get_plotter(PCB_FILENAME, temp_dir) as plotter:
        plotter.plot_options.SetDrillMarksType(pcbnew.PCB_PLOT_PARAMS.NO_DRILL_SHAPE)

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
            output_filename = plotter.plot(layer.layer, pcbnew.PLOT_FORMAT_GERBER)
            desired_name = '%s.%s' % (
                board_name,
                layer.extension,
            )
            desired_output_filename = os.path.join(output_directory, desired_name)
            os.rename(output_filename, desired_output_filename)

        drill_file, map_file = plotter.plot_drill()
        desired_drill_file = os.path.join(output_directory, '%s.txt' % (board_name,))
        desired_map_file = os.path.join(output_directory, '%s.drill.pdf' % (board_name,))
        os.rename(drill_file, desired_drill_file)
        os.rename(map_file, desired_map_file)

if __name__ == '__main__':
    run()

