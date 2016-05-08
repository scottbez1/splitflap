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

import logging
import os
import shutil

import openscad
from svg_processor import SvgProcessor

class Renderer(object):
    def __init__(self, input_file, output_folder, extra_variables=None):
        self.input_file = input_file
        self.output_folder = output_folder
        if extra_variables is None:
            extra_variables = {}
        self.extra_variables = extra_variables

    def clean(self):
        shutil.rmtree(self.output_folder, ignore_errors=True)
        os.makedirs(self.output_folder)

    def _get_variables(self, variables):
         v = self.extra_variables.copy()
         v.update(variables)
         return v

    def _get_num_components(self):
        stdout, stderr = openscad.run(
            self.input_file,
            os.path.join(self.output_folder, 'dummy.png'),
            output_size=[1,1],
            variables = self._get_variables({
                'render_3d': False,
                'render_index': 0,
            }),
            capture_output=True,
        )
        return openscad.extract_values(stderr)['num_components']

    def _get_component_file(self, i):
        return os.path.join(self.output_folder, 'component_%05d.svg' % i)

    def _render_component(self, i):
        output_file = self._get_component_file(i)
        for style in ('cut', 'etch'):
            logging.debug('Rendering component %d, %s', i, style)
            try:
                stdout, stderr = openscad.run(
                        self.input_file,
                        output_file,
                        variables = self._get_variables({
                            'render_3d': False,
                            'render_index': i,
                            'render_etch': style == 'etch',
                        }),
                        capture_output=True,
                    )
            except openscad.OpenSCADException as e:
                if 'Current top level object is not a 2D object.' in e.stderr:
                    # This is expected if we try rendering an etch layer as a
                    # cut, since there will be nothing to export
                    continue
                else:
                    raise

            processor = SvgProcessor(output_file)
            processor.fix_dimens()
            if style == 'cut':
                processor.apply_laser_cut_style()
            elif style == 'etch':
                processor.apply_laser_etch_style()
            break
        else:
            raise ValueError("Invalid component!", i)
        return processor

    def render_svgs(self):
        num_components = int(self._get_num_components())
        logging.info('Found %d components to render', num_components)
        svg_output = None
        for i in range(num_components):
            svg_processor = self._render_component(i)
            if svg_output is None:
                svg_output = svg_processor
            else:
                svg_output.import_paths(svg_processor)
        output_file_path = os.path.join(self.output_folder, 'combined.svg')
        svg_output.write(output_file_path)
        return output_file_path


