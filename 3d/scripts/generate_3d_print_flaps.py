#!/usr/bin/env python3

#   Copyright 2025 Scott Bezek and the splitflap contributors
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
import sys

from multiprocessing.dummy import Pool
from shutil import (
    rmtree
)

from colored_stl_exporter import ColoredStlExporter

script_dir = os.path.dirname(os.path.abspath(__file__))
source_parts_dir = os.path.dirname(script_dir)
repo_root = os.path.dirname(source_parts_dir)
sys.path.append(repo_root)

from util import (
    file_util
)

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser()
    parser.add_argument('-f', '--font', type=str, help='Name of font preset to use - see flap_fonts.scad')
    parser.add_argument('-t', '--text', type=str, help='String of text to generate')
    parser.add_argument('--vertical-keepout-clip', action='store_true', help='Clip letters that violate vertical keepout zones')
    args = parser.parse_args()

    extra_variables = {}
    if args.font is not None:
        extra_variables['font_preset'] = args.font
    if args.text is not None:
        extra_variables['character_list'] = args.text

    if args.vertical_keepout_clip:
        extra_variables['vertical_keepout_mode'] = 2

    output_directory = os.path.join(source_parts_dir, 'build', 'flap_3dp')
    rmtree(output_directory, ignore_errors=True)

    def render_flap(flap_number):
        flap_path = os.path.join(output_directory, f"flap_{flap_number}")
        file_util.mkdir_p(flap_path)
        openscad_variables = {
            "flap_number": flap_number,
        }
        openscad_variables.update(extra_variables)

        ColoredStlExporter(os.path.join(source_parts_dir, 'flap_3dp.scad'), flap_path, openscad_variables=openscad_variables).run()

    pool = Pool()
    num_flaps = len(args.text) if args.text is not None else 52
    for _ in pool.imap_unordered(render_flap, range(num_flaps)):
        # Consume results as they occur so any exception is rethrown
        pass
    pool.close()
    pool.join()
