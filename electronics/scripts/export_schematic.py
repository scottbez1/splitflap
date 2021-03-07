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
import re
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
    versioned_file,
    xdotool,
    wait_for_window,
    recorded_xvfb,
)

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

def eeschema_plot_schematic(output_directory):
    wait_for_window('eeschema', '\[')

    logger.info('Focus main eeschema window')
    xdotool(['search', '--name', '\[', 'windowfocus'])

    logger.info('Open File->Plot->Plot')
    xdotool(['key', 'alt+f'])
    xdotool(['key', 'l'])

    wait_for_window('plot', 'Plot')
    xdotool(['search', '--name', 'Plot', 'windowfocus'])

    logger.info('Enter build output directory')
    xdotool(['type', output_directory])

    logger.info('Plot')
    xdotool(['key', 'Return'])

    logger.info('Wait before shutdown')
    time.sleep(2)

def export_schematic(schematic_file):
    # Use absolute path - eeschema handles libraries differently with full path vs filename
    schematic_file = os.path.abspath(schematic_file)
    filename, _ = os.path.splitext(os.path.basename(schematic_file))
    output_dir = os.path.join(electronics_root, 'build')
    file_util.mkdir_p(output_dir)

    screencast_output_file = os.path.join(output_dir, 'export_schematic_screencast.ogv')
    schematic_output_pdf_file = os.path.join(output_dir, f'{filename}.pdf')
    schematic_output_png_file = os.path.join(output_dir, f'{filename}.png')

    settings = {
        'PlotFormat': '4',  # PDF
    }
    with patch_config(os.path.expanduser('~/.config/kicad/eeschema'), settings):
        with versioned_file(schematic_file):
            with recorded_xvfb(screencast_output_file, width=800, height=600, colordepth=24):
                with PopenContext(['eeschema', schematic_file], close_fds=True) as eeschema_proc:
                    eeschema_plot_schematic(output_dir)
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


@contextmanager
def patch_config(filename, replacements):
    with open(filename, 'r') as f:
        original_contents = f.read()
    
    new_contents = original_contents
    for (key, value) in replacements.items():
        pattern = '^' + re.escape(key) + '=(.*)$'
        new_contents = re.sub(pattern, f'{key}={value}', new_contents, flags=re.MULTILINE)

    with open(filename, 'w') as f:
        logger.debug('Writing to %s', filename)
        f.write(new_contents)
    try:
        yield
    finally:
        with open(filename, 'w') as f:
            logger.debug('Restoring %s', filename)
            f.write(original_contents)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('schematic')
    args = parser.parse_args()
    export_schematic(args.schematic)

