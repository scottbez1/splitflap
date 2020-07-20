#!/usr/bin/env python3

#   Copyright 2015-2020 Scott Bezek and the splitflap contributors
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
import psutil
import sys
import time

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

WIDTH = 2560
HEIGHT = 1440

RENDER_TIMEOUT = 4 * 60


def _wait_for_pcbnew_idle():
    start = time.time()
    while time.time() < start + RENDER_TIMEOUT:
        for proc in psutil.process_iter():
            if proc.name() == 'pcbnew':
                cpu = proc.cpu_percent(interval=1)
                print(cpu)
                if cpu < 5:
                    print('Render took %d seconds' % (time.time() - start))
                    return
        time.sleep(1)
    raise RuntimeError('Timeout waiting for pcbnew to go idle')


def _pcbnew_export_3d(output_directory):
    output_file = os.path.join(output_directory, '3d.png')
    if os.path.exists(output_file):
        os.remove(output_file)

    wait_for_window('pcbnew', 'Pcbnew ')

    logger.info('Focus main pcbnew window')
    xdotool(['search', '--name', 'Pcbnew ', 'windowfocus'])

    logger.info('Open 3d viewer')
    xdotool(['key', 'alt+3'])

    wait_for_window('3D Viewer', '3D Viewer')
    xdotool(['search', '--name', '3D Viewer', 'windowfocus'])

    time.sleep(3)

    xdotool(['search', '--name', '3D Viewer', 'windowmove', '0', '0'])
    xdotool(['search', '--name', '3D Viewer', 'windowsize', str(WIDTH), str(HEIGHT)])

    logger.info('Zoom in')
    xdotool([
        'key',
        'alt+v',
        'i',
    ])
    time.sleep(5)

    for i in range(2):
        logger.info('Rotate X Clockwise')
        xdotool([
            'key',
            'alt+v',
            'Down',
            'Down',
            'Down',
            'Down',
            'Return',
        ])
        time.sleep(2)

    for i in range(2):
        logger.info('Rotate Y counter-clockwise')
        xdotool([
            'key',
            'alt+v',
            'Down',
            'Down',
            'Down',
            'Down',
            'Down',
            'Down',
            'Down',
            'Return',
        ])
        time.sleep(2)

    logger.info('Rotate Z counter-clockwise')
    xdotool([
        'key',
        'alt+v',
        'Down',
        'Down',
        'Down',
        'Down',
        'Down',
        'Down',
        'Down',
        'Down',
        'Down',
        'Return',
    ])
    time.sleep(2)

    logger.info('Wait for rendering...')

    _wait_for_pcbnew_idle()

    time.sleep(5)

    logger.info('Export current view')
    xdotool([
        'key',
        'alt+f',
        'Return',
    ])

    logger.info('Enter build output filename')
    xdotool(['type', output_file])

    logger.info('Save')
    xdotool(['key', 'Return'])

    logger.info('Wait before shutdown')
    time.sleep(2)


def export_schematic():
    pcb_file = os.path.join(electronics_root, 'splitflap.kicad_pcb')
    output_dir = os.path.join(electronics_root, 'build')
    file_util.mkdir_p(output_dir)

    screencast_output_file = os.path.join(output_dir, 'export_3d_screencast.ogv')

    with versioned_file(pcb_file):
        with recorded_xvfb(screencast_output_file, width=3840, height=2160, colordepth=24):
            with PopenContext(['pcbnew', pcb_file], close_fds=True) as pcbnew_proc:
                _pcbnew_export_3d(output_dir)
                pcbnew_proc.terminate()


if __name__ == '__main__':
    export_schematic()

