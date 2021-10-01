from cobs import cobs
from contextlib import contextmanager
import os
import serial
import serial.tools.list_ports
import six
import sys
from threading import Thread
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
    
    def setPositions(self, indexes):
        message = splitflap_pb2.ToSplitflap()
        # message.splitflap_command = splitflap_pb2.SplitflapCommand()
        for i in indexes:
            param = 0
            if i < 0:
                action = splitflap_pb2.SplitflapCommand.ModuleCommand.NO_OP
            else:
                action = splitflap_pb2.SplitflapCommand.ModuleCommand.GO_TO_FLAP
                param = i
            command = splitflap_pb2.SplitflapCommand.ModuleCommand()
            command.action = action
            command.param = param
            message.splitflap_command.modules.append(command)
        
        payload = bytearray(message.SerializeToString())

        crc = zlib.crc32(payload) & 0xffffffff
        payload.append(crc & 0xff)
        payload.append((crc >> 8) & 0xff)
        payload.append((crc >> 16) & 0xff)
        payload.append((crc >> 24) & 0xff)

        encoded = cobs.encode(payload)

        self.serial.write(encoded)
        self.serial.write(b'\0')



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
        t = Thread(target=s._loop_for_status)
        t.start()
        while True:
            input()
            s.setPositions([1, 2, 3])
