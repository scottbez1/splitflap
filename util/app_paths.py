

import os
from shutil import which
from sys import platform

APP_PATH_OVERRIDES = {
    # If your programs are not found or installed in a non-standard location, add an override
    # like this:
    # 'openscad': 'C:\Program Files (x86)\OpenSCAD\openscad.exe'
}

def get(name):
    if name in APP_PATH_OVERRIDES:
        return APP_PATH_OVERRIDES[name]

    if platform == "linux" or platform == "linux2":
        # Linux: we assume applications are installed on user's PATH
        _check_path(name, which(name))
        return which(name)
    elif platform == "darwin":
        # OS X
        raise NotImplementedError('Mac OS is not yet supported')
    elif platform == "win32":
        # Windows: check standard locations
        paths = {
            'inkscape': 'C:\\Program Files\\Inkscape\\bin\\inkscape.exe',
            'openscad': 'C:\\Program Files\\OpenSCAD\\openscad.exe',
        }
        if name in paths:
            _check_path(name, paths[name])
            return paths[name]
        raise RuntimeError(f'Unknown program {name!r} - please add an entry to {os.path.abspath(__file__)!r}')

def _check_path(name, path):
    if not os.path.exists(path):
        raise RuntimeError(f'Expected {name!r} at {path!r}, but it wasn\'t found. If {name!r} is installed in a nonstandard location, add an entry to APP_PATH_OVERRIDES in {os.path.abspath(__file__)!r}')