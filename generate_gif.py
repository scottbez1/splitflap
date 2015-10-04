#!/usr/bin/env python
from __future__ import division
from __future__ import print_function

import errno
import numbers
import os
import shutil
import subprocess

def run_openscad(
        input_file,
        output_file,
        output_size=None,
        camera_translation=None,
        camera_rotation=None,
        camera_distance=None,
        variables=None,
        ):
    if output_size is None:
        output_size = [320, 240]

    command = [
        'openscad',
        '-o', output_file,
        '--imgsize=%d,%d' % tuple(output_size),
    ]

    if camera_translation is not None or camera_rotation is not None or camera_distance is not None:
        if camera_translation is None:
            camera_translation = [0, 0, 0]
        if camera_rotation is None:
            camera_rotation = [60, 0, 135]
        if camera_distance is None:
            camera_distance = 600
        command += ['--camera=%f,%f,%f,%f,%f,%f,%f' % tuple(camera_translation + camera_rotation + [camera_distance])]

    if variables is not None:
        for k, v in variables.items():
            if isinstance(v, basestring):
                value = '"%s"' % v.replace('"', '\\"')
            elif v is True:
                value = 'true'
            elif v is False:
                value = 'false'
            elif isinstance(v, numbers.Integral):
                value = '%d' % v
            elif isinstance(v, numbers.Real):
                value = '%f' % v
            else:
                raise ValueError("Unknown value type", type(v), v)
            command += ['-D', '%s=%s' % (k, value)]

    command += [input_file]

    print(command)
    subprocess.check_call(command)

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
    for i in range(num_frames):
        angle = 135 + i * 360 / num_frames
        run_openscad(
            'pulley_assembly.scad',
            os.path.join(output_folder, 'frame_%05d.png' % (start_frame + i)),
            output_size = [320, 240],
            camera_translation = [0, 0, 0],
            camera_rotation = [60, 0, angle],
            camera_distance = 600,
            variables = variables,
        )

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
