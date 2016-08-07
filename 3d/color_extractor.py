#!/usr/bin/env python

#   Copyright 2015 Scott Bezek
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
import hashlib
import logging
import os
import re

USE_INCLUDE_REGEX = re.compile(r'\b(?P<statement>use|include)\s*<\s*(?P<filename>.*?)\s*>\s*;')
COLOR_REGEX = re.compile(r'\bcolor\s*\(')
DIR = os.path.dirname(__file__)

logger = logging.getLogger(__name__)


def run(input_file):
    output_folder = os.path.join(DIR, 'build', 'modified_project')
    try:
        os.makedirs(output_folder)
    except OSError as e:
        if e.errno == errno.EEXIST and os.path.isdir(output_folder):
            pass
        else:
            raise

    color_values = set()

    def replace_with_color_collector(contents):
        return contents + '''
module color_extractor(c) {
    echo(extracted_color=c);
    children();
}
        '''

    walk_scad_files(input_file, replace_with_color_collector, output_folder)


def walk_scad_files(input_file, mutate_function, output_folder):
    visited = set()
    to_process = [input_file]
    while len(to_process):
        current_file = to_process.pop(0)
        logger.debug('Processing {}'.format(current_file))

        with open(current_file, 'rb') as f:
            contents = f.read()
            for include in USE_INCLUDE_REGEX.finditer(contents):
                next_filename = os.path.realpath(include.group('filename'))
                logger.debug('-- found {}'.format(next_filename))
                if next_filename.lower().endswith('.scad') and next_filename not in visited:
                    to_process.append(next_filename)
                    visited.add(next_filename)

        def replace(match):
            if match.group('filename').lower().endswith('.scad'):
                return '{} <{}>;'.format(match.group('statement'), get_transformed_file_path(match.group('filename')))
            else:
                return

        contents = mutate_function(USE_INCLUDE_REGEX.sub(replace, contents))

        with open(os.path.join(output_folder, get_transformed_file_path(current_file)), 'wb') as out_file:
            out_file.write(contents)


def get_transformed_file_path(original_path):
    return hashlib.sha256(os.path.realpath(original_path)).hexdigest() + '.scad'


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    run(os.path.join(DIR, 'splitflap.scad'))
