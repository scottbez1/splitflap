from __future__ import print_function

import random
import time

from splitflap import splitflap
from splitflap import ask_for_serial_port

words = [
    'duck', 'goat', 'lion', 'bear', 'mole', 'crab',
    'hare', 'toad', 'wolf', 'lynx', 'cats', 'dogs',
    'bees', 'mule', 'seal', 'bird', 'frog', 'deer',
    'lamb', 'fish', 'hawk', 'kiwi',
]


def run():
    port = ask_for_serial_port()

    print('Starting...')
    with splitflap(port) as s:
        while True:
            word = random.choice(words)
            print('Going to \'{}\''.format(word))
            status = s.set_text(word)
            s.print_status(status)
            time.sleep(10)


if __name__ == '__main__':
    run()
