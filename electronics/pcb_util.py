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

import argparse
import datetime
import logging
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
    #TODO: add the rest
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

def _get_versioned_contents(filename):
    with open(filename, 'rb') as pcb:
        original_contents = pcb.read()
        version_info = _get_version_info()
        return original_contents \
            .replace('COMMIT: deadbeef', 'COMMIT: ' + version_info['revision']) \
            .replace('DATE: YYYY-MM-DD', 'DATE: ' + version_info['date'])

def _get_version_info():
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
 
