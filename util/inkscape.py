
import logging
import re
import subprocess

import util.app_paths as app_paths

_inkscape_version_cache = None

logger = logging.getLogger(__name__)

def _version():
    global _inkscape_version_cache
    if _inkscape_version_cache is not None:
        return _inkscape_version_cache
    
    info = subprocess.check_output([
        app_paths.get('inkscape'),
        '--version'
    ]).decode('utf-8')

    m = re.match(r'Inkscape (\d+\.\d+)', info)
    _inkscape_version_cache = float(m.group(1))
    logger.debug(f'Found Inkscape version {_inkscape_version_cache}')
    return _inkscape_version_cache

def without_gui():
    if _version() >= 1:
        """
        The --without-gui option has been removed. Most parameters trigger Inkscape to run without GUI by default now.
        Instead enable GUI mode specifically using --with-gui if needed.
        """
        return []
    else:
        return ['--without-gui']

def _export_filename_v1(filename):
    assert _version() >= 1
    return [f'--export-filename={filename}']

def export_png(filename):
    if _version() >= 1:
        return _export_filename_v1(filename)
    else:
        return ['--export-png', filename]

def export_pdf(filename):
    if _version() >= 1:
        return _export_filename_v1(filename)
    else:
        return ['--export-pdf', filename]
