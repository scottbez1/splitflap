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
        try:
            self.etch_enabled = self.extra_variables['render_etch']
        except KeyError:
            self.etch_enabled = True

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
                '_is_projection_rendering': True,
                'render_3d': False,
                'render_index': 0,
            }),
            capture_output=True,
        )
        outputs = openscad.extract_values(stderr)
        for key, value in outputs.items():
            if key.startswith('debug_'):
                logging.debug('DEBUG VALUE %r = %r', key, value)
        return outputs['num_components']

    def _get_component_file(self, i):
        return os.path.join(self.output_folder, 'component_%05d.svg' % i)

    def _render_component(self, i, panel_horizontal, panel_vertical):
        output_file = self._get_component_file(i)
        style_options = ['cut']
        if self.etch_enabled:
            style_options.append('etch')
        for style in (style_options):
            logging.debug('Rendering component %d, %s', i, style)
            try:
                _ = openscad.run(
                        self.input_file,
                        output_file,
                        variables=self._get_variables({
                            '_is_projection_rendering': True,
                            'render_3d': False,
                            'render_index': i,
                            'render_etch': style == 'etch',
                            'panel_horizontal': panel_horizontal,
                            'panel_vertical': panel_vertical,
                        }),
                        capture_output=True,
                    )
            except openscad.OpenSCADException as e:
                if b'Current top level object is not a 2D object.' in e.stderr:
                    # This is expected if we try rendering an etch layer as a
                    # cut, since there will be nothing to export
                    continue
                else:
                    raise

            processor = SvgProcessor(output_file)
            if style == 'cut':
                processor.apply_laser_cut_style()
            elif style == 'etch':
                processor.apply_laser_etch_style()
            return processor

        logging.debug('Component %d has no geometry', i)
        return None

    def render_svgs(self, panelize_quantity):
        assert panelize_quantity == 1 or panelize_quantity % 2 == 0, 'Panelize quantity must be 1 or an even number'
        num_components = int(self._get_num_components())
        logging.info('Found %d components to render', num_components)
        svg_output = None

        horizontal_range = 1 if panelize_quantity == 1 else 2
        vertical_range = (panelize_quantity + 1) // 2
        for panel_horizontal in range(0, horizontal_range):
            for panel_vertical in range(0, vertical_range):
                for i in range(num_components):
                    svg_processor = self._render_component(i, panel_horizontal, panel_vertical)
                    if svg_output is None:
                        svg_output = svg_processor
                    elif svg_processor is not None:
                        svg_output.import_paths(svg_processor)
        output_file_path = os.path.join(self.output_folder, 'combined.svg')
        svg_output.write(output_file_path)
        return output_file_path


