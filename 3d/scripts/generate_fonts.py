#!/usr/bin/env python

#   Copyright 2020 Scott Bezek and the splitflap contributors
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

import argparse
import logging
import os
import subprocess
import sys

from svg_processor import SvgProcessor
from projection_renderer import Renderer

script_dir = os.path.dirname(os.path.abspath(__file__))
source_parts_dir = os.path.dirname(script_dir)
repo_root = os.path.dirname(source_parts_dir)
sys.path.append(repo_root)

from util import rev_info

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser()
    parser.add_argument('--kerf', type=float, help='Override kerf_width value')
    parser.add_argument('--skip-optimize', action='store_true', help='Don\'t remove redundant/overlapping cut lines')

    args = parser.parse_args()

    output_directory = os.path.join(source_parts_dir, 'build', 'fonts')

    extra_variables = {
        'render_revision': rev_info.git_short_rev(),
        'render_date': rev_info.current_date(),
    }
    if args.kerf is not None:
        extra_variables['kerf_width'] = args.kerf

    renderer = Renderer(os.path.join(source_parts_dir, 'flap_fonts.scad'), output_directory, extra_variables)
    renderer.clean()
    svg_output = renderer.render_svgs(panelize_quantity = 1)

    processor = SvgProcessor(svg_output)

    redundant_lines, merged_lines = None, None
    if not args.skip_optimize:
        logging.info('Removing redundant lines')
        redundant_lines, merged_lines = processor.remove_redundant_lines()

    processor.write(svg_output)

    logging.info('\n\n\nDone rendering to SVG: ' + svg_output)
