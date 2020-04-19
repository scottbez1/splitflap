from __future__ import print_function

import random
import six
import time

import serial
import serial.tools.list_ports

from splitflap import splitflap

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
            print('Going to {}'.format(word))
            status = s.set_text(word)
            print_status(status)
            time.sleep(10)


def ask_for_serial_port():
    print('Available ports:')
    ports = sorted(
        filter(
            lambda p: p.description != 'n/a',
            serial.tools.list_ports.comports(),
        ),
        key=lambda p: p.device,
    )
    for i, port in enumerate(ports):
        print('[{: 2}] {} - {}'.format(i, port.device, port.description))
    print()
    value = six.moves.input('Use which port? ')
    port_index = int(value)
    assert 0 <= port_index < len(ports)
    return ports[port_index].device


def print_status(status):
    for module in status:
        state = ''
        if module['state'] == 'panic':
            state = '!!!!'
        elif module['state'] == 'look_for_home':
            state = '...'
        elif module['state'] == 'sensor_error':
            state = '????'
        elif module['state'] == 'normal':
            state = module['flap']
        print('{:4}  {: 4} {: 4}'.format(state, module['count_missed_home'], module['count_unexpected_home']))


if __name__ == '__main__':
    run()
