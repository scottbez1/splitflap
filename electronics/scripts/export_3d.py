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
import psutil
import sys
import time

electronics_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
repo_root = os.path.dirname(electronics_root)
sys.path.append(repo_root)

from util import file_util
from export_util import (
    patch_config,
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
                print(cpu, flush=True)
                if cpu < 5:
                    print('Render took %d seconds' % (time.time() - start))
                    return
        time.sleep(1)
    raise RuntimeError('Timeout waiting for pcbnew to go idle')


def _pcbnew_export_3d(output_file):
    if os.path.exists(output_file):
        os.remove(output_file)

    wait_for_window('pcbnew', 'Pcbnew ')

    time.sleep(1)

    logger.info('Focus main pcbnew window')
    xdotool(['search', '--name', 'Pcbnew ', 'windowfocus'])

    time.sleep(1)

    logger.info('Open 3d viewer')
    xdotool(['key', 'alt+3'])

    wait_for_window('3D Viewer', '3D Viewer')
    xdotool(['search', '--name', '3D Viewer', 'windowfocus'])

    time.sleep(3)

    # Maximize window
    xdotool(['search', '--name', '3D Viewer', 'windowmove', '0', '0'])
    xdotool(['search', '--name', '3D Viewer', 'windowsize', str(WIDTH), str(HEIGHT)])

    time.sleep(3)

    logger.info('Zoom in')
    for i in range(6):
        xdotool([
            'click',
            '4',
        ])
        time.sleep(0.2)

    logger.info('Move right')
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
        'Down',
        'Down',
        'Return',
    ])
    time.sleep(2)

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
    xdotool([
        'key',
        'ctrl+a',
    ])
    xdotool(['type', output_file])

    logger.info('Save')
    xdotool(['key', 'Return'])

    logger.info('Wait before shutdown')
    time.sleep(2)


def export_3d(filename):
    pcb_file = os.path.abspath(filename)
    output_dir = os.path.join(electronics_root, 'build')
    file_util.mkdir_p(output_dir)

    screencast_output_file = os.path.join(output_dir, 'export_3d_screencast.ogv')

    name, _ = os.path.splitext(os.path.basename(pcb_file))
    output_file = os.path.join(output_dir, f'{name}-3d.png')

    settings = {
        'canvas_type': '1',
        'SMaskColor_Red': '0.1',
        'SMaskColor_Green': '0.1',
        'SMaskColor_Blue': '0.1',
        'RenderEngine': '1',
        'Render_RAY_ProceduralTextures': '0',
    }
    with patch_config(os.path.expanduser('~/.config/kicad/pcbnew'), settings):
        with versioned_file(pcb_file):
            with recorded_xvfb(screencast_output_file, width=2560, height=1440, colordepth=24):
                with PopenContext(['pcbnew', pcb_file], close_fds=True) as pcbnew_proc:
                    _pcbnew_export_3d(output_file)
                    pcbnew_proc.terminate()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('pcb')
    args = parser.parse_args()
    export_3d(args.pcb)
