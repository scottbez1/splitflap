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
import subprocess
import psutil
import sys
import time

from contextlib import contextmanager

electronics_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
repo_root = os.path.dirname(electronics_root)
sys.path.append(repo_root)

from util import file_util
from export_util import (
    PopenContext,
    versioned_schematic,
    xdotool,
    wait_for_window,
    recorded_xvfb,
)

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

class PlotTimeoutError(Exception):
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return repr(self.msg)

def eeschema_plot_schematic(output_directory):
    wait_for_window('eeschema', '\[')

    logger.info('Focus main eeschema window')
    xdotool(['search', '--name', '\[', 'windowfocus'])

    logger.info('Open File->Plot->Plot')
    xdotool(['key', 'alt+f'])
    xdotool(['key', 'p'])
    xdotool(['key', 'p'])

    wait_for_window('plot', 'Plot')
    xdotool(['search', '--name', 'Plot', 'windowfocus'])

    logger.info('Enter build output directory')
    xdotool(['type', output_directory])

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

    logger.info('Plot')
    xdotool(['key', 'Return'])

def process_has_file_open(process, file_path):
    for file in process.open_files():
        if file.path == file_path:
           return True
    return False

def wait_for_plot(eeschema_proc, plot_file, timeout=2):
    begin = time.time()
    ps = psutil.Process(eeschema_proc.pid)
    while not process_has_file_open(ps, plot_file):
        if time.time() > begin + timeout:
            logger.error('Timed out waiting for eeschema to open plot file')
            return False
    while process_has_file_open(ps, plot_file):
        if time.time() > begin + timeout:
            logger.error('Timed out waiting for eeschema to close plot file')
            return False
    logger.info('splitflap.pdf created')
    return True

def export_schematic():
    schematic_file = os.path.join(electronics_root, 'splitflap.sch')
    output_dir = os.path.join(electronics_root, 'build')
    file_util.mkdir_p(output_dir)
    plot_file = os.path.join(output_dir, 'splitflap.pdf')

    screencast_output_file = os.path.join(output_dir, 'export_schematic_screencast.ogv')
    schematic_output_pdf_file = os.path.join(output_dir, 'splitflap.pdf')
    schematic_output_png_file = os.path.join(output_dir, 'schematic.png')

    with versioned_schematic(schematic_file):
        with recorded_xvfb(screencast_output_file, width=800, height=600, colordepth=24):
            with PopenContext(['eeschema', schematic_file], close_fds=True) as eeschema_proc:
                eeschema_plot_schematic(output_dir)
                logger.info('Wait before shutdown')
                plot_created = wait_for_plot(eeschema_proc, plot_file)
                eeschema_proc.terminate()

    if not plot_created: return

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
    export_schematic()

