#!/usr/bin/env python

import datetime
import subprocess

def git_short_rev():
    return subprocess.check_output([
        'git',
        'rev-parse',
        '--short',
        'HEAD',
    ]).strip()

def current_date():
    return datetime.date.today().strftime('%Y-%m-%d')

