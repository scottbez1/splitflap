#!/usr/bin/env python
from __future__ import division
from __future__ import print_function

import errno
import functools
import logging
import os
import shutil

from multiprocessing.dummy import Pool

import openscad


class Renderer(object):
    def __init__(self, input_file, output_folder):
        self.input_file = input_file
        self.output_folder = output_folder

    def clean(self):
        shutil.rmtree(self.output_folder, ignore_errors=True)
    def _get_num_components(self):
        stdout, stderr = openscad.run(
            self.input_file,
            os.path.join(self.output_folder, 'dummy.png'),
            output_size=[1,1],
            variables = {
                'render_3d': False,
                'render_index': 0,
            },
            capture_output=True,
        )
        return openscad.extract_values(stderr)['num_components']

    def _render_component(self, i):
        stdout, stderr = openscad.run(
                self.input_file,
                os.path.join(self.output_folder, 'component_%05d.svg' % i),
                variables = {
                    'render_3d': False,
                    'render_index': i,
                },
                capture_output=True,
            )

    def render_svgs(self):
        num_components = int(self._get_num_components())
        logging.info('Found %d components to render', num_components)
        for i in range(num_components):
            logging.debug('Rendering component %d', i)
            self._render_component(i)

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    renderer = Renderer('splitflap.scad', os.path.join('build', 'laser_parts'))
    renderer.clean()
    renderer.render_svgs()

