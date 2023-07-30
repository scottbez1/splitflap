#!/usr/bin/env python3
#   Copyright 2021 Scott Bezek and the splitflap contributors
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

from pathlib import Path

import os
import shutil
import subprocess
import sys

def run():
    SCRIPT_PATH = Path(__file__).absolute().parent
    REPO_ROOT = SCRIPT_PATH.parent

    proto_path = REPO_ROOT / 'proto'

    nanopb_path = REPO_ROOT / 'thirdparty' / 'nanopb'

    # Make sure nanopb submodule is available
    if not os.path.isdir(nanopb_path):
        print(f'Nanopb checkout not found! Make sure you have inited/updated the submodule located at {nanopb_path}', file=sys.stderr)
        exit(1)

    nanopb_generator_path = nanopb_path / 'generator' / 'nanopb_generator.py'
    c_generated_output_path = REPO_ROOT / 'arduino' / 'splitflap' / 'esp32' / 'proto_gen'
    
    proto_files = [f for f in os.listdir(proto_path) if f.endswith('.proto')]
    assert len(proto_files) > 0, 'No proto files found!'

    # Generate C files via nanopb
    subprocess.check_call(['python3', nanopb_generator_path, '-D', c_generated_output_path] + proto_files, cwd=proto_path)


    # Use nanopb's packaged protoc to generate python bindings
    protoc_path = nanopb_path / 'generator' / 'protoc'
    python_generated_output_path = REPO_ROOT / 'software' / 'chainlink' / 'proto_gen'
    python_generated_output_path.mkdir(parents=True, exist_ok=True)
    subprocess.check_call([
        protoc_path,
        '--python_out',
        python_generated_output_path,
    ] + proto_files, cwd=proto_path)

    # Copy nanopb's compiled options proto
    shutil.copy2(nanopb_path / 'generator' / 'proto' / 'nanopb_pb2.py', python_generated_output_path)
    

if __name__ == '__main__':
    run()
