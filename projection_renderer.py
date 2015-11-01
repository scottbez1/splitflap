from __future__ import division
from __future__ import print_function

import logging
import os
import shutil

import openscad
from svg_processor import SvgProcessor

class Renderer(object):
    def __init__(self, input_file, output_folder):
        self.input_file = input_file
        self.output_folder = output_folder

    def clean(self):
        shutil.rmtree(self.output_folder, ignore_errors=True)
        os.makedirs(self.output_folder)

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

    def _get_component_file(self, i):
        return os.path.join(self.output_folder, 'component_%05d.svg' % i)

    def _render_component(self, i):
        output_file = self._get_component_file(i)
        stdout, stderr = openscad.run(
                self.input_file,
                output_file,
                variables = {
                    'render_3d': False,
                    'render_index': i,
                },
                capture_output=True,
            )
        processor = SvgProcessor(output_file)
        processor.fix_dimens()
        processor.delete_registration_mark()
        processor.apply_laser_cut_style()
        return processor

    def render_svgs(self):
        num_components = int(self._get_num_components())
        logging.info('Found %d components to render', num_components)
        svg_output = None
        for i in range(num_components):
            logging.debug('Rendering component %d', i)
            svg_processor = self._render_component(i)
            if svg_output is None:
                svg_output = svg_processor
            else:
                svg_output.import_paths(svg_processor)
        output_file_path = os.path.join(self.output_folder, 'combined.svg')
        svg_output.write(output_file_path)
        return output_file_path


