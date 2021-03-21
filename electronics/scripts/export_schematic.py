#!/usr/bin/env python3

#   Copyright 2015-2021 Scott Bezek and the splitflap contributors
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
import subprocess
import sys
import time

from contextlib import contextmanager

electronics_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
repo_root = os.path.dirname(electronics_root)
sys.path.append(repo_root)

from util import file_util
from export_util import (
    PopenContext,
    patch_config,
    versioned_file,
    xdotool,
    wait_for_window,
    recorded_xvfb,
)

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

EESCHEMA_CONFIG_PATH = os.path.expanduser('~/.config/kicad/eeschema')

WIDTH = 800
HEIGHT = 600

def eeschema_plot_schematic(output_directory, kicad_4):
    wait_for_window('eeschema', '\[', additional_commands=['windowfocus'])

    logger.info('Open File->Plot->Plot')
    xdotool(['key', 'alt+f'])
    if kicad_4:
        xdotool(['key', 'p'])
        xdotool(['key', 'p'])
    else:
        xdotool(['key', 'l'])

    wait_for_window('plot', 'Plot', additional_commands=['windowfocus'])

    time.sleep(2)
    
    if not kicad_4:
        # Move/resize window to standard position and click into the text box
        xdotool(['search', '--name', 'Plot', 'windowmove', '0', '0'])
        xdotool(['search', '--name', 'Plot', 'windowsize', str(WIDTH), str(HEIGHT)])
        time.sleep(2)
        xdotool(['mousemove', '400', '20', 'click', '1'])

    logger.info('Enter build output directory')
    xdotool(['key', 'BackSpace', 'BackSpace'])
    xdotool(['type', output_directory])

    time.sleep(2)

    if kicad_4:
        logger.info('Select PDF plot format')
        xdotool([
            'key',
            'Tab',
            'Tab',
            'Tab',
            'Tab',
            'Tab',
            'Up',
            'Up',
            'Up',
            'space',
        ])
        time.sleep(2)


    logger.info('Plot')
    xdotool(['key', 'Return'])

    logger.info('Wait before shutdown')
    time.sleep(5)

def export_schematic(schematic_file, kicad_4):
    # Use absolute path - eeschema handles libraries differently with full path vs filename
    schematic_file = os.path.abspath(schematic_file)
    filename, _ = os.path.splitext(os.path.basename(schematic_file))
    output_dir = os.path.join(electronics_root, 'build')
    file_util.mkdir_p(output_dir)

    screencast_output_file = os.path.join(output_dir, 'export_schematic_screencast.ogv')
    schematic_output_pdf_file = os.path.join(output_dir, filename + '.pdf')
    schematic_output_png_file = os.path.join(output_dir, filename + '.png')

    settings = {
        'PlotFormat': '4',  # PDF
    }
    with patch_config(os.path.expanduser('~/.config/kicad/eeschema'), settings):
        with versioned_file(schematic_file):
            with recorded_xvfb(screencast_output_file, width=WIDTH, height=HEIGHT, colordepth=24):
                with PopenContext(['eeschema', schematic_file], close_fds=True) as eeschema_proc:
                    eeschema_plot_schematic(output_dir, kicad_4)
                    eeschema_proc.terminate()

    logger.info('Rasterize')
    subprocess.check_call([
        'convert',
        '-density', '96',
        schematic_output_pdf_file,
       '-background', 'white',
       '-alpha', 'remove',
       schematic_output_png_file,
   ])


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('schematic')
    parser.add_argument('--kicad-4', action='store_true')
    args = parser.parse_args()
    export_schematic(args.schematic, args.kicad_4)

