from __future__ import division
from __future__ import print_function

import logging
import numbers
import re
import subprocess

logger = logging.getLogger(__name__)

def run(
        input_file,
        output_file,
        output_size=None,
        camera_translation=None,
        camera_rotation=None,
        camera_distance=None,
        variables=None,
        capture_output=False,
        ):

    command = [
        'openscad',
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

    if variables is not None:
        for k, v in variables.items():
            if isinstance(v, basestring):
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
    proc = subprocess.Popen(command, stdout=stdout_type, stderr=stderr_type)
    stdout, stderr = proc.communicate()
    returncode = proc.returncode

    logger.debug('returncode:%d', returncode)
    logger.debug(stderr)
    if returncode != 0:
        raise ValueError('openscad returned non-zero!', returncode)
    return stdout, stderr

def extract_values(stderr):
    """
    Extracts values from ECHO statements as a dictionary. Values are strings.
    """
    result = {}
    pattern = re.compile(r'^ECHO: (.+) = (.+)$')
    for line in stderr.splitlines():
        m = pattern.search(line)
        if m:
            result[m.group(1)] = m.group(2)
    return result

