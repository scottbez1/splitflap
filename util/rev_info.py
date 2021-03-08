import datetime
import subprocess

def git_short_rev():
    return subprocess.check_output([
        'git',
        'rev-parse',
        '--short',
        'HEAD',
    ]).decode('utf-8').strip()

def current_date():
    return datetime.date.today().strftime('%Y-%m-%d')

