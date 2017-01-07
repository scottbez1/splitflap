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


from __future__ import division
from __future__ import print_function

import errno
import logging
import os

import openscad

logging.basicConfig(level=logging.DEBUG)

output_folder = os.path.join('build')

try:
    os.makedirs(output_folder)
except OSError as e:
    if e.errno == errno.EEXIST and os.path.isdir(output_folder):
        pass
    else:
        raise

openscad.run(
    'splitflap.scad',
    os.path.join(output_folder, 'snapshot.png'),
    output_size = [1280, 1024],
    camera_translation = [0, 0, 0],
    camera_rotation = [60, 0, 135],
    camera_distance = 600,
    colorscheme = 'Nature',
)

