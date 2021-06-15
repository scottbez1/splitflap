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

try:
    import webcolors
except ImportError:
    webcolors = None

from multiprocessing.dummy import Pool

import openscad

USE_INCLUDE_REGEX = re.compile(r'\b(?P<statement>use|include)\s*<\s*(?P<filename>.*?)\s*>\s*;')
COLOR_REGEX = re.compile(r'\bcolor\s*\(')
EXTRACTED_COLOR_REGEX = re.compile(r'ECHO: extracted_color = (?P<color>.*)')

# RGB_COLOR_REGEX = re.compile(r'\[(?P<r>.*?),(?P<g>.*?),(?P<b>.*?)\]')
RGBA_COLOR_REGEX = re.compile(r'\[(?P<r>[0-9.]*), *?(?P<g>[0-9.]*), *?(?P<b>[0-9.]*)(?:, *)?(?P<a>[0-9.]*)?\]')


class ColoredStlExporter(object):

    def __init__(self, input_file, build_folder, openscad_variables = None):
        self.logger = logging.getLogger(__name__)
        self.input_file = input_file
        self.intermediate_folder = os.path.join(build_folder, 'intermediate')
        self.output_folder = os.path.join(build_folder, 'colored_stl')
        if openscad_variables is None:
            openscad_variables = {}
        self.openscad_variables = openscad_variables

    def run(self):
        mkdir_p(self.intermediate_folder)
        mkdir_p(self.output_folder)

        color_values = self._extract_colors()
        self.logger.debug('Found {} unique colors: {}'.format(len(color_values), color_values))

        manifest = {}

        def render_color(color):
            file_name = self._export_stl(color)
            if file_name is not None:
                manifest[file_name] = ColoredStlExporter.parse_openscad_color(color)

        pool = Pool()
        for _ in pool.imap_unordered(render_color, color_values):
            # Consume results as they occur so any exception is rethrown
            pass
        pool.close()
        pool.join()

        with open(os.path.join(self.output_folder, 'manifest.json'), 'w') as f:
            f.write(json.dumps(manifest, indent=4))

    def _extract_colors(self):
        """Returns a list of color expressions used within the input_file or any dependencies thereof"""
        self.logger.info("Extracting color information...")

        # Create a mutator that defines a color_extractor() module that prints the passed color argument and replaces
        # all color(xyz) declarations with color_extractor(xyz) modules instances.
        def replace_with_color_collector(contents):
            contents = COLOR_REGEX.sub(' color_extractor(', contents)
            return contents + '''
module color_extractor(c) {
    echo(extracted_color=c);
    children();
}'''

        intermediate_subfolder = os.path.join(self.intermediate_folder, 'color_extraction')
        self.walk_and_mutate_scad_files(replace_with_color_collector, intermediate_subfolder)

        # Compile the mutated scad model and collect the echo output for processing
        echo_file = os.path.join(self.intermediate_folder, 'color_extractor.echo')
        openscad.run(
            os.path.join(intermediate_subfolder, ColoredStlExporter.get_transformed_file_path(self.input_file)),
            echo_file,
            variables=self.openscad_variables,
            capture_output=True
        )

        # Parse the color values from the output
        color_values = set()
        with open(echo_file, 'r') as f:
            for line in f:
                match = EXTRACTED_COLOR_REGEX.search(line)
                if match:
                    color_values.add(match.group('color'))
        return color_values

    def _export_stl(self, color):
        """Exports an .stl file containing only objects of the specified color from the input model"""

        # Create a mutator that defines a color_selector() module that conditionally includes its children only if the
        # passed color argument is the color we're currently trying to export.
        def replace_with_color_selector(contents):
            contents = COLOR_REGEX.sub(' color_selector(', contents)
            return contents + '''
        module color_selector(c) {{
            precision = 0.0000001;  // arbitrary
            function compare_floats(x, y, i=0) = 
                  (len(x) != len(y)) ? false  // if arrays differ in length, they can't be equal
                : (i >= len(x)) ? true  // if we've hit the end of the arrays without issue, we're equal
                : (x[i] - precision <= y[i]) && x[i] + precision >= y[i]
                    ? compare_floats(x, y, i+1)
                    : false;  // not equal, short circuit

            if (c == {0} || compare_floats(c, {0}))
                children();
        }}
                    '''.format(color)

        color_hash = hashlib.sha256(color.encode('utf-8')).hexdigest()

        intermediate_subfolder = os.path.join(self.intermediate_folder, 'color_' + color_hash)
        self.walk_and_mutate_scad_files(replace_with_color_selector, intermediate_subfolder)

        # Name the stl model after its color (but use a hash function to make sure it's a valid filename)
        file_name = color_hash + '.stl'

        self.logger.info('Exporting STL for color {} as {}...'.format(color, file_name))

        try:
            openscad.run(
                os.path.join(intermediate_subfolder, ColoredStlExporter.get_transformed_file_path(self.input_file)),
                os.path.join(self.output_folder, file_name),
                variables=self.openscad_variables,
                capture_output=True
            )
        except openscad.OpenSCADException as e:
            if b'Current top level object is empty.' in e.stderr:
                # No geometry in this color
                return None
            else:
                raise

        return file_name

    def walk_and_mutate_scad_files(self, mutate_function, intermediate_subfolder):
        mkdir_p(intermediate_subfolder)
        visited = set()
        to_process = [self.input_file]
        while len(to_process):
            current_file = to_process.pop(0)
            self.logger.debug('Processing {}'.format(current_file))

            with open(current_file, 'rb') as f:
                contents = f.read()

            # Only process .scad files; copy any other file types (e.g. fonts) over as-is
            if current_file.lower().endswith('.scad'):
                contents = contents.decode('utf-8')
                current_folder = os.path.dirname(current_file)
                for include in USE_INCLUDE_REGEX.finditer(contents):
                    next_filename = os.path.realpath(
                        os.path.join(current_folder, include.group('filename')))
                    if next_filename not in visited:
                        to_process.append(next_filename)
                        visited.add(next_filename)

                def replace(match):
                    return '{} <{}>;'.format(match.group('statement'),
                                             ColoredStlExporter.get_transformed_file_path(
                                                 os.path.join(current_folder, match.group('filename'))))

                contents = mutate_function(USE_INCLUDE_REGEX.sub(replace, contents)).encode('utf-8')

            with open(os.path.join(intermediate_subfolder,
                                   ColoredStlExporter.get_transformed_file_path(current_file)), 'wb') as out_file:
                out_file.write(contents)

    @staticmethod
    def get_transformed_file_path(original_path):
        extension = os.path.splitext(original_path)[1]
        return hashlib.sha256(os.path.realpath(original_path).encode('utf-8')).hexdigest() + extension

    @staticmethod
    def parse_openscad_color(color):
        match = RGBA_COLOR_REGEX.search(color)
        if match:
            color_out = [
                float(match.group('r')),
                float(match.group('g')),
                float(match.group('b')),
            ]
            if(match.group('a')):
               color_out.append(float(match.group('a')))

            return color_out

        if '"' in color and webcolors:
            try:
                c = webcolors.name_to_rgb(color[1:-1]) # skip the ""
                return c.red/255., c.green/255., c.blue/255.
            except ValueError:
                pass

        raise ValueError('Failed to parse color. Must be named webcolor or in [<r>, <g>, <b>] format. {}'.format(color))


def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as e:
        if e.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('input_file', type=str, help='OpenSCAD file to parse into STLs')
    parser.add_argument('output_folder', type=str, help='directory to place the colored STL files')

    args = parser.parse_args()

    ColoredStlExporter(args.input_file, args.output_folder).run()
