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

from collections import namedtuple

import pcb_util

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

PCB_FILENAME = 'splitflap.kicad_pcb'

# Have to use absolute path for build_directory otherwise pcbnew will output relative to the temp file
BUILD_DIRECTORY = os.path.abspath('build')

def run():
    temp_dir = os.path.join(BUILD_DIRECTORY, 'temp_pdfs')
    shutil.rmtree(temp_dir, ignore_errors=True)
    try:
        os.makedirs(temp_dir)
        plot_to_directory(BUILD_DIRECTORY, temp_dir)
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)

def plot_to_directory(output_directory, temp_dir):
    board_name = os.path.splitext(os.path.basename(PCB_FILENAME))[0]

    with pcb_util.get_plotter(PCB_FILENAME, temp_dir) as plotter:
        plotter.plot_options.SetDrillMarksType(pcbnew.PCB_PLOT_PARAMS.NO_DRILL_SHAPE)
        plotter.plot_options.SetExcludeEdgeLayer(False)

        LayerDef = namedtuple('LayerDef', ['layer', 'mirror'])
        layers = [
            LayerDef(pcbnew.F_Cu, False),
            LayerDef(pcbnew.B_Cu, True),
            LayerDef(pcbnew.F_SilkS, False),
            LayerDef(pcbnew.B_SilkS, True),
        ]

        pdfs = []
        for layer in layers:
            plotter.plot_options.SetMirror(layer.mirror)
            output_filename = plotter.plot(layer.layer, pcbnew.PLOT_FORMAT_PDF)
            pdfs.append(output_filename)

        _, map_file = plotter.plot_drill()
        pdfs.append(map_file)

        output_pdf_filename = os.path.join(output_directory, '%s_packet.pdf' % (board_name,))

        command = ['pdfunite'] + pdfs + [output_pdf_filename]
        subprocess.check_call(command)

if __name__ == '__main__':
    run()

