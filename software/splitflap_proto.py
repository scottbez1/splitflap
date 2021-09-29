from cobs import cobs
from contextlib import contextmanager
import os
import serial
import serial.tools.list_ports
import six
import sys
import zlib

software_root = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(software_root, 'proto_gen'))

from proto_gen import splitflap_pb2


class Splitflap(object):

    def __init__(self, serial_instance):
        self.serial = serial_instance

    def _loop_for_status(self):
        while True:
            buffer = self.serial.read_until(b'\0')
            if not len(buffer):
                continue
            
            try:
                decoded = cobs.decode(buffer[:-1])
            except cobs.DecodeError:
                print(f'Failed decode ({len(buffer)} bytes)')
                print(buffer)
                continue

            if len(decoded) < 4:
                continue

            payload = decoded[:-4]
            expected_crc = zlib.crc32(payload) & 0xffffffff
            provided_crc = (decoded[-1] << 24) \
                            | (decoded[-2] << 16) \
                            | (decoded[-3] << 8) \
                            | decoded[-4]
            
            if expected_crc != provided_crc:
                print(f'Bad CRC. expected={hex(expected_crc)}, actual={hex(provided_crc)}')
                continue

            message = splitflap_pb2.FromSplitflap()
            message.ParseFromString(payload)
            print(message)



@contextmanager
def splitflap(serial_port):
    with serial.Serial(serial_port, 921600, timeout=1.0) as ser:
        s = Splitflap(ser)
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


if __name__ == '__main__':
    p = ask_for_serial_port()
    with splitflap(p) as s:
        s._loop_for_status()