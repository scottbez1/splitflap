import datetime
import subprocess

def git_short_rev():
    try:
        return subprocess.check_output([
            'git',
            'rev-parse',
            '--short',
            'HEAD',
        ]).decode('utf-8').strip()
    except Exception as e:
        print("Exception: " + type(e).__name__ + " while trying to find git revision")
        return "notfound"

def current_date():
    return datetime.date.today().strftime('%Y-%m-%d')

