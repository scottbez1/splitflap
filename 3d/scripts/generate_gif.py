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

import errno
import functools
import logging
import os
import shutil
import subprocess

from multiprocessing.dummy import Pool

import openscad

logging.basicConfig(level=logging.DEBUG)

NUM_FLAPS = 52 # TODO: read from source?

def generate_gif(output_folder, delay, filename):
    command = [
        'convert',
        os.path.join(output_folder, 'frames', 'frame*.png'),
        '-set', 'delay', str(delay),
        os.path.join(output_folder, filename),
    ]
    logging.debug(command)
    subprocess.check_call(command)

def render_rotation(input_file, output_folder, num_frames, start_frame, variables):
    def render_frame(i):
        angle = 135 + i * 360 / num_frames
        openscad.run(
            input_file,
            os.path.join(output_folder, 'frames', 'frame_%05d.png' % (start_frame + i)),
            output_size = [320, 320],
            camera_translation = [0, 0, -15],
            camera_rotation = [60, 0, angle],
            camera_distance = 600,
            variables = variables,
            colorscheme = 'DeepOcean',
        )
    pool = Pool()
    for _ in pool.imap_unordered(render_frame, range(num_frames)):
        # Consume results as they occur so any exception is rethrown
        pass
    pool.close()
    pool.join()

def render_flaps(input_file, output_folder, variables):
    def render_flap(i):
        merged_variables = {}
        merged_variables.update(variables)
        merged_variables.update({'render_flap_index': i})
        openscad.run(
            input_file,
            os.path.join(output_folder, 'frames', 'frame_%05d.png' % (i)),
            output_size = [600,800],
            camera_translation = [-5, 0, -9],
            camera_rotation = [90, 0, 180],
            camera_distance = 500,
            variables = merged_variables,
            colorscheme = 'Starnight',
        )
    pool = Pool()
    for _ in pool.imap_unordered(render_flap, range(NUM_FLAPS)):
        # Consume results as they occur so any exception is rethrown
        pass
    pool.close()
    pool.join()

script_dir = os.path.dirname(os.path.abspath(__file__))
source_parts_dir = os.path.dirname(script_dir)

input_file = os.path.join(source_parts_dir, 'splitflap.scad')
output_folder = os.path.join(source_parts_dir, 'build', 'animation')

shutil.rmtree(output_folder, ignore_errors=True)
frames_folder = os.path.join(output_folder, 'frames')
os.makedirs(frames_folder)

num_frames = 50
render_rotation(input_file, output_folder, num_frames, 0, {
    'render_enclosure': 2,
    'render_flaps': 2,
})
render_rotation(input_file, output_folder, num_frames, num_frames, {
    'render_enclosure': 1,
    'render_flaps': 2,
})
generate_gif(output_folder, '1x15', 'animation.gif')

shutil.rmtree(frames_folder, ignore_errors=True)
os.makedirs(frames_folder)

render_flaps(input_file, output_folder, {
    'render_enclosure': 2,
    'render_flaps': 2,
})

generate_gif(output_folder, '20', 'all_flaps.gif')
