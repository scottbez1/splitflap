#!/usr/bin/env python
#
#   * Corey Goldberg, 2012, 2013, 2015, 2016
#
#   * inspired by: PyVirtualDisplay


"""wrapper for running display inside X virtual framebuffer (Xvfb)"""


import os
import fnmatch
import subprocess
import tempfile
import time


class Xvfb:

    def __init__(self, width=800, height=680, colordepth=24, **kwargs):
        self.width = width
        self.height = height
        self.colordepth = colordepth

        if not self.xvfb_exists():
            msg = 'Can not find Xvfb. Please install it and try again.'
            raise EnvironmentError(msg)

        self.extra_xvfb_args = ['-screen', '0', '{}x{}x{}'.format(
                                self.width, self.height, self.colordepth)]

        for key, value in kwargs.items():
            self.extra_xvfb_args += ['-{}'.format(key), value]

        if 'DISPLAY' in os.environ:
            self.orig_display = os.environ['DISPLAY'].split(':')[1]
        else:
            self.orig_display = None

        self.proc = None

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop()

    def start(self):
        self.new_display = self._get_next_unused_display()
        display_var = ':{}'.format(self.new_display)
        self.xvfb_cmd = ['Xvfb', display_var] + self.extra_xvfb_args
        with open(os.devnull, 'w') as fnull:
            self.proc = subprocess.Popen(self.xvfb_cmd,
                                         stdout=fnull,
                                         stderr=fnull,
                                         preexec_fn=os.setpgrp, #NOTE(sbezek): added this to avoid Xvfb handling SIGINT before python
                                         close_fds=True)
        time.sleep(0.1)  # give Xvfb time to start
        ret_code = self.proc.poll()
        if ret_code is None:
            self._set_display_var(self.new_display)
        else:
            raise RuntimeError('Xvfb did not start')

    def stop(self):
        if self.orig_display is None:
            del os.environ['DISPLAY']
        else:
            self._set_display_var(self.orig_display)
        if self.proc is not None:
            try:
                self.proc.terminate()
                self.proc.wait()
            except OSError:
                pass
            self.proc = None

    def _get_next_unused_display(self):
        tmpdir = tempfile.gettempdir()
        pattern = '.X*-lock'
        lockfile_names = fnmatch.filter(os.listdir(tmpdir), pattern)
        existing_displays = [int(name.split('X')[1].split('-')[0])
                             for name in lockfile_names]
        highest_display = max(existing_displays) if existing_displays else 0
        return highest_display + 1

    def _set_display_var(self, display):
        os.environ['DISPLAY'] = ':{}'.format(display)

    def xvfb_exists(self):
        """Check that Xvfb is available on PATH and is executable."""
        paths = os.environ['PATH'].split(os.pathsep)
        return any(os.access(os.path.join(path, 'Xvfb'), os.X_OK)
                   for path in paths)
