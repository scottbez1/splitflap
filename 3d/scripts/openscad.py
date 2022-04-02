#   Copyright 2015-2020 Scott Bezek and the splitflap contributors
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
import numbers
import os
import re
import subprocess
import sys

repo_root = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
sys.path.append(repo_root)

from util import app_paths

logger = logging.getLogger(__name__)


class OpenSCADException(Exception):
    def __init__(self, message, returncode, stdout=None, stderr=None):
        truncated_stdout = '\n'.join(stdout.decode('utf-8').splitlines()[-20:]) if stdout is not None else None
        truncated_stderr = '\n'.join(stderr.decode('utf-8').splitlines()[-20:]) if stderr is not None else None
        super(OpenSCADException, self).__init__('%s\n\nRETURN CODE:%d\n\nSTDOUT:\n%s\n\nSTDERR:\n%s' % (
            message, returncode, truncated_stdout, truncated_stderr))
        self.returncode = returncode
        self.stdout = stdout
        self.stderr = stderr


def run(
        input_file,
        output_file,
        output_size=None,
        camera_translation=None,
        camera_rotation=None,
        camera_distance=None,
        variables=None,
        capture_output=False,
        colorscheme=None,
        ):

    command = [
        app_paths.get('openscad'),
        '-o', output_file,
    ]

    if output_size is not None:
        command += ['--imgsize=%d,%d' % tuple(output_size)]

    if camera_translation is not None or camera_rotation is not None or camera_distance is not None:
        if camera_translation is None:
            camera_translation = [0, 0, 0]
        if camera_rotation is None:
            camera_rotation = [60, 0, 135]
        if camera_distance is None:
            camera_distance = 600
        command += ['--camera=%f,%f,%f,%f,%f,%f,%f' % tuple(camera_translation + camera_rotation + [camera_distance])]
    if colorscheme is not None:
        command += ['--colorscheme=%s' % colorscheme]

    if variables is not None:
        for k, v in variables.items():
            if isinstance(v, str) or isinstance(v, bytes):
                try:
                    v = v.decode('utf-8')
                except (UnicodeDecodeError, AttributeError):
                    pass
                value = '"%s"' % v.replace('"', '\\"')
            elif v is True:
                value = 'true'
            elif v is False:
                value = 'false'
            elif isinstance(v, numbers.Integral):
                value = '%d' % v
            elif isinstance(v, numbers.Real):
                value = '%f' % v
            else:
                raise ValueError("Unknown value type", type(v), v)
            command += ['-D', '%s=%s' % (k, value)]

    command += [input_file]

    logger.info(command)

    stdout_type = subprocess.PIPE if capture_output else None
    stderr_type = subprocess.PIPE if capture_output else None
    try:
        proc = subprocess.Popen(command, stdout=stdout_type, stderr=stderr_type)
        stdout, stderr = proc.communicate()
        returncode = proc.returncode
    except OSError as e:
        if 'No such file or directory' in e.strerror:
            raise RuntimeError('Could not find openscad! Make sure you\'ve installed OpenSCAD and it\'s on your PATH '
                               'environment variable')
        else:
            raise

    logger.debug('returncode:%d', returncode)
    if returncode != 0:
        raise OpenSCADException('openscad returned non-zero!', returncode, stdout=stdout, stderr=stderr)
    return stdout, stderr


def extract_values(stderr):
    """
    Extracts values from ECHO statements as a dictionary. Values are strings.
    """
    result = {}
    pattern = re.compile(r'^ECHO: (.+) = (.+)$')
    for line in stderr.splitlines():
        m = pattern.search(line.decode('utf-8'))
        if m:
            result[m.group(1)] = m.group(2)
    return result
