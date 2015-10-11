#!/usr/bin/env python
from __future__ import division
from __future__ import print_function

import errno
import functools
import logging
import os
import shutil
import subprocess

from multiprocessing.dummy import Pool

import openscad

logging.basicConfig(level=logging.DEBUG)

def generate_gif(output_folder):
    command = [
        'convert',
        os.path.join(output_folder, 'frame*.png'),
        '-set', 'delay', '1x15',
        os.path.join(output_folder, 'animation.gif'),
    ]
    print(command)
    subprocess.check_call(command)

def render_rotation(output_folder, num_frames, start_frame, variables):
    def render_frame(i):
        angle = 135 + i * 360 / num_frames
        openscad.run(
            'splitflap.scad',
            os.path.join(output_folder, 'frame_%05d.png' % (start_frame + i)),
            output_size = [320, 240],
            camera_translation = [0, 0, 0],
            camera_rotation = [60, 0, angle],
            camera_distance = 600,
            variables = variables,
        )
    pool = Pool(4)
    pool.map(render_frame, range(num_frames))
    pool.close()
    pool.join()

output_folder = os.path.join('build', 'animation')

shutil.rmtree(output_folder, ignore_errors=True)
os.makedirs(output_folder)

num_frames = 50
render_rotation(output_folder, num_frames, 0, {
    'render_enclosure': 2,
    'render_flaps': True,
})
render_rotation(output_folder, num_frames, num_frames, {
    'render_enclosure': 1,
    'render_flaps': True,
})
render_rotation(output_folder, num_frames, num_frames*2, {
    'render_enclosure': 0,
    'render_flaps': False,
})


generate_gif(output_folder)
