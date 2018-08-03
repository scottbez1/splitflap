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

import argparse
import datetime
import logging
import os
import pcbnew
import subprocess
import tempfile

from contextlib import contextmanager

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

_LAYER_NAME = {
    pcbnew.F_Cu: 'F.Cu',
    pcbnew.B_Cu: 'B.Cu',
    pcbnew.F_Adhes: 'F.Adhes',
    pcbnew.B_Adhes: 'B.Adhes',
    pcbnew.F_SilkS: 'F.SilkS',
    pcbnew.B_SilkS: 'B.SilkS',
    pcbnew.F_Paste: 'F.Paste',
    pcbnew.B_Paste: 'B.Paste',
    pcbnew.F_Mask: 'F.Mask',
    pcbnew.B_Mask: 'B.Mask',
    pcbnew.Edge_Cuts: 'Edge.Cuts',
    # TODO: add the rest
}


@contextmanager
def versioned_board(filename):
    versioned_contents = _get_versioned_contents(filename)
    with tempfile.NamedTemporaryFile(suffix='.kicad_pcb') as temp_pcb:
        logger.debug('Writing to %s', temp_pcb.name)
        temp_pcb.write(versioned_contents)
        temp_pcb.flush()

        logger.debug('Load board')
        board = pcbnew.LoadBoard(temp_pcb.name)
        yield board


def get_layer_name(kicad_layer_id):
    if kicad_layer_id in _LAYER_NAME:
        return _LAYER_NAME[kicad_layer_id]
    else:
        return 'Unknown(%r)' % (kicad_layer_id,)


@contextmanager
def get_plotter(pcb_filename, build_directory):
    with versioned_board(pcb_filename) as board:
        yield Plotter(board, build_directory)


class Plotter(object):
    def __init__(self, board, build_directory):
        self.board = board
        self.build_directory = build_directory
        self.plot_controller = pcbnew.PLOT_CONTROLLER(board)
        self.plot_options = self.plot_controller.GetPlotOptions()
        self.plot_options.SetOutputDirectory(build_directory)

        self.plot_options.SetPlotFrameRef(False)
        self.plot_options.SetLineWidth(pcbnew.FromMM(0.35))
        self.plot_options.SetScale(1)
        self.plot_options.SetUseAuxOrigin(True)
        self.plot_options.SetMirror(False)
        self.plot_options.SetExcludeEdgeLayer(True)

    def plot(self, layer, plot_format):
        logger.info('Plotting layer %s (kicad layer=%r)', get_layer_name(layer), layer)
        layer_name = get_layer_name(layer)
        self.plot_controller.SetLayer(layer)
        self.plot_controller.OpenPlotfile(layer_name, plot_format , 'Plot')
        output_filename = self.plot_controller.GetPlotFileName()
        self.plot_controller.PlotLayer()
        self.plot_controller.ClosePlot()
        return output_filename

    def plot_drill(self):
        board_name = os.path.splitext(os.path.basename(self.board.GetFileName()))[0]
        logger.info('Plotting drill file')
        drill_writer = pcbnew.EXCELLON_WRITER(self.board)
        drill_writer.SetMapFileFormat(pcbnew.PLOT_FORMAT_PDF)

        mirror = False
        minimalHeader = False
        offset = pcbnew.wxPoint(0, 0)
        merge_npth = True
        drill_writer.SetOptions(mirror, minimalHeader, offset, merge_npth)

        metric_format = True
        drill_writer.SetFormat(metric_format)

        generate_drill = True
        generate_map = True
        drill_writer.CreateDrillandMapFilesSet(self.build_directory, generate_drill, generate_map)

        drill_file_name = os.path.join(
            self.build_directory,
            '%s.drl' % (board_name,)
        )

        map_file_name = os.path.join(
            self.build_directory,
            '%s-drl_map.pdf' % (board_name,)
        )
        return drill_file_name, map_file_name


def _get_versioned_contents(filename):
    with open(filename, 'rb') as pcb:
        original_contents = pcb.read()
        version_info = get_version_info()
        return original_contents \
            .replace('COMMIT: deadbeef', 'COMMIT: ' + version_info['revision']) \
            .replace('DATE: YYYY-MM-DD', 'DATE: ' + version_info['date'])


def get_version_info():
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

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Test pcb util')
    parser.add_argument('input_file', help='Input .kicad_pcb file')
    args = parser.parse_args()
    with versioned_board(args.input_file) as board:
        logger.info('Loaded %s', board.GetFileName())
        for module in board.GetModules():
            logger.info('Module %s: %s', module.GetReference(), module.GetValue())
