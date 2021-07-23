import json
from contextlib import contextmanager

import serial
import serial.tools.list_ports

import six

_ALPHABET = {
    ' ',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '.',
    ',',
    '\'',
}


class Splitflap(object):

    def __init__(self, serial_instance):
        self.serial = serial_instance

        self.has_inited = False
        self.num_modules = 0
        self.character_list = _ALPHABET

        self.last_command = None
        self.last_status = None
        self.exception = None

    def _loop_for_status(self):
        while True:
            line = self.serial.readline().lstrip(b'\0').rstrip(b'\n')
            if not len(line):
                continue
            try:
                data = json.loads(line)
            except:
                print('Failed to parse', line)
                raise
            t = data['type']
            if t == 'init':
                if self.has_inited:
                    raise RuntimeError('Unexpected re-init!')
                self.has_inited = True
                self.num_modules = data['num_modules']
                try:
                    self.character_list = data['character_list']
                except KeyError:
                    self.character_list = _ALPHABET  # for compatibility

            elif t == 'move_echo':
                if not self.has_inited:
                    raise RuntimeError('Got move_echo before init!')
                if self.last_command is None:
                    raise RuntimeError('Unexpected move_echo response from controller')
                if self.last_command != data['dest']:
                    raise RuntimeError('Bad response from controller. Expected {!r} but got {!r}'.format(
                        self.last_command,
                        data['dest'],
                    ))
            elif t == 'status':
                if not self.has_inited:
                    raise RuntimeError('Got status before init!')
                if len(data['modules']) != self.num_modules:
                    raise RuntimeError('Wrong number of modules in status update. Expected {} but got {}'.format(
                        self.num_modules,
                        len(data['modules']),
                    ))
                self.last_status = data['modules']
                return self.last_status
            elif t == 'no_op':
                pass
            else:
                raise RuntimeError('Unexpected message: {!r}'.format(data))

    def in_character_list(self, letter):
        return letter in self.character_list

    def get_character_list(self):
        return self.character_list

    def set_text(self, text):
        text = text[0:self.num_modules]  # trim to number of modules available
        for letter in text:
            assert self.in_character_list(letter), 'Unexpected letter: {!r}. Must be one of {!r}'.format(
                letter,
                list(self.character_list),
            )
        self.last_command = text
        self.serial.write(b'=' + text.encode() + b'\n')
        return self._loop_for_status()

    def recalibrate_all(self):
        self.serial.write(b'@')
        return self._loop_for_status()

    def get_text(self):
        text = ""
        if self.last_status != None:
            for module in self.last_status:
                text += module['flap']
        return text

    def get_status(self):
        return self.last_status

    def get_num_modules(self):
        return self.num_modules

    def print_status(self, status=None):
        if(status is None):
            status = self.last_status

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


@contextmanager
def splitflap(serial_port):
    with serial.Serial(serial_port, 38400, timeout=1.0) as ser:
        s = Splitflap(ser)
        s._loop_for_status()
        yield s


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
