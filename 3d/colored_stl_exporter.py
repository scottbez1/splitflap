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

"""Exports each color of a given OpenSCAD model as a separate .stl file.

First, determines the set of unique colors used in the model by replacing usage of the 'color()' statement with a
custom module that echos the color value when compiled (must be done at compilation time rather than simply parsing the
.scad file since colors may be expressed as variables or other complex expressions). Note: only colors in the rgb array
format ('[0.2, 0.1, 0.8]') are supported.

Then, for each color, generates a model that replaces usage of 'color()' with a custom module that conditionally renders
its children if the color argument matches the current color we're exporting. This model is run through OpenSCAD to
generate an .stl file. Once all single-color .stl files have been exported, a manifest json file is generated that maps
each .stl filename to its rgb color info."""

from __future__ import division
from __future__ import print_function

import errno
import hashlib
import json
import logging
import os
import re

import openscad

STANDARD_RENDER_VARIABLES = {
    'render_3d': True,
    'render_enclosure': 2,
    'render_flaps': True,
    'render_units': 1,
    'render_pcb': True,
}

USE_INCLUDE_REGEX = re.compile(r'\b(?P<statement>use|include)\s*<\s*(?P<filename>.*?)\s*>\s*;')
COLOR_REGEX = re.compile(r'\bcolor\s*\(')
EXTRACTED_COLOR_REGEX = re.compile(r'ECHO: extracted_color = (?P<color>.*)')

RGB_COLOR_REGEX = re.compile(r'\[(?P<r>.*?),(?P<g>.*?),(?P<b>.*?)\]')

DIR = os.path.dirname(__file__)

logger = logging.getLogger(__name__)


def run(input_file):
    build_folder = os.path.join(DIR, 'build')
    intermediate_folder = os.path.join(build_folder, 'modified_project')
    mkdir_p(intermediate_folder)
    output_folder = os.path.join(build_folder, 'colored_stl')
    mkdir_p(output_folder)

    color_values = extract_colors(input_file, intermediate_folder)
    logger.debug('Found {} unique colors: {}'.format(len(color_values), color_values))

    manifest = {}
    for color in color_values:
        file_name = export_stl(input_file, intermediate_folder, output_folder, color)
        manifest[file_name] = parse_openscad_color(color)

    with open(os.path.join(output_folder, 'manifest.json'), 'wb') as f:
        f.write(json.dumps(manifest, indent=4))


def parse_openscad_color(color):
    match = RGB_COLOR_REGEX.search(color)
    if not match:
        raise ValueError('Failed to parse color. Must be in [<r>, <g>, <b>] format. {}'.format(color))
    return [
        float(match.group('r')),
        float(match.group('g')),
        float(match.group('b')),
    ]

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as e:
        if e.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


def extract_colors(input_file, intermediate_folder):
    logger.info("Extracting color information...")

    def replace_with_color_collector(contents):
        contents = COLOR_REGEX.sub(' color_extractor(', contents)
        return contents + '''
module color_extractor(c) {
    echo(extracted_color=c);
    children();
}
            '''

    walk_scad_files(input_file, replace_with_color_collector, intermediate_folder)

    echo_file = os.path.join(intermediate_folder, 'color_extractor.echo')
    openscad.run(
        os.path.join(intermediate_folder, get_transformed_file_path(input_file)),
        echo_file,
        variables=STANDARD_RENDER_VARIABLES,
        capture_output=True
    )

    color_values = set()
    with open(echo_file, 'rb') as f:
        for line in f:
            match = EXTRACTED_COLOR_REGEX.search(line)
            if match:
                color_values.add(match.group('color'))
    return color_values


def export_stl(input_file, intermediate_folder, output_folder, color):
    def replace_with_color_selector(contents):
        contents = COLOR_REGEX.sub(' color_selector(', contents)
        return contents + '''
module color_selector(c) {{
    if (c == {})
        children();
}}
            '''.format(color)

    walk_scad_files(input_file, replace_with_color_selector, intermediate_folder)

    file_name = hashlib.sha256(color).hexdigest() + '.stl'

    logger.info('Exporting STL for color {} as {}...'.format(color, file_name))

    openscad.run(
        os.path.join(intermediate_folder, get_transformed_file_path(input_file)),
        os.path.join(output_folder, file_name),
        variables=STANDARD_RENDER_VARIABLES,
        capture_output=True
    )

    return file_name


def walk_scad_files(input_file, mutate_function, intermediate_folder):
    visited = set()
    to_process = [input_file]
    while len(to_process):
        current_file = to_process.pop(0)
        logger.debug('Processing {}'.format(current_file))

        with open(current_file, 'rb') as f:
            contents = f.read()
            for include in USE_INCLUDE_REGEX.finditer(contents):
                next_filename = os.path.realpath(include.group('filename'))
                if next_filename.lower().endswith('.scad') and next_filename not in visited:
                    to_process.append(next_filename)
                    visited.add(next_filename)

        def replace(match):
            if match.group('filename').lower().endswith('.scad'):
                return '{} <{}>;'.format(match.group('statement'), get_transformed_file_path(match.group('filename')))
            else:
                return

        contents = mutate_function(USE_INCLUDE_REGEX.sub(replace, contents))

        with open(os.path.join(intermediate_folder, get_transformed_file_path(current_file)), 'wb') as out_file:
            out_file.write(contents)


def get_transformed_file_path(original_path):
    return hashlib.sha256(os.path.realpath(original_path)).hexdigest() + '.scad'


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    run(os.path.join(DIR, 'splitflap.scad'))
