#!/usr/bin/env python3

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

import logging
import os
import sys

from colored_stl_exporter import ColoredStlExporter

script_dir = os.path.dirname(os.path.abspath(__file__))
source_parts_dir = os.path.dirname(script_dir)
repo_root = os.path.dirname(source_parts_dir)

sys.path.append(repo_root)

from util import rev_info


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    openscad_variables = {
        'render_3d': True,
        'render_enclosure': 2,
        'render_flaps': 2,
        'render_units': 1,
        'render_pcb': True,
        'render_revision': rev_info.git_short_rev(),
        'render_date': rev_info.current_date(),
    }

    exporter = ColoredStlExporter(
        os.path.join(source_parts_dir, 'splitflap.scad'),
        os.path.join(source_parts_dir, 'build'),
        openscad_variables)
    exporter.run()
