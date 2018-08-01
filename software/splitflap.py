import json
from contextlib import contextmanager

import serial

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
        self.last_command = None
        self.last_status = None
        self.exception = None

    def _loop_for_status(self):
        while True:
            line = self.serial.readline().lstrip('\0').rstrip('\n')
            data = json.loads(line)
            t = data['type']
            if t == 'init':
                if self.has_inited:
                    raise RuntimeError('Unexpected re-init!')
                self.has_inited = True
                self.num_modules = data['num_modules']
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

    def is_in_alphabet(self, letter):
        return letter in _ALPHABET

    def set_text(self, text):
        for letter in text:
            assert self.is_in_alphabet(letter), 'Unexpected letter: {!r}. Must be one of {!r}'.format(
                letter,
                list(_ALPHABET),
            )
        self.last_command = text
        self.serial.write('={}\n'.format(text))
        return self._loop_for_status()

    def recalibrate_all(self):
        self.serial.write('@')
        return self._loop_for_status()

    def get_status(self):
        return self.last_status


@contextmanager
def splitflap(serial_port):
    with serial.Serial(serial_port, 38400) as ser:
        s = Splitflap(ser)
        s._loop_for_status()
        yield s
