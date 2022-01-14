from cobs import cobs
from contextlib import contextmanager
import logging
import os
from queue import (
    Empty,
    Full,
    Queue,
)
import serial
import serial.tools.list_ports
import six
import sys
from threading import (
    Thread,
    Lock,
)
import time
import zlib

software_root = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(software_root, 'proto_gen'))

from proto_gen import splitflap_pb2


class Splitflap(object):

    RETRY_TIMEOUT = 2 # TODO: set to 0.25

    def __init__(self, serial_instance):
        self._serial = serial_instance
        self._logger = logging.getLogger('splitflap')
        self._out_q = Queue()
        self._ack_q = Queue()
        self._next_nonce = 1
        self._run = True

        self._lock = Lock()
        self._message_handler = None

    def _read_loop(self):
        self._logger.debug('Read loop started')
        buffer = b''
        while True:
            buffer += self._serial.read_until(b'\0')
            if not self._run:
                return

            if not len(buffer):
                continue

            if not buffer.endswith(b'\0'):
                continue
            
            self._process_frame(buffer[:-1])
            buffer = b''

    def _process_frame(self, frame):
        try:
            decoded = cobs.decode(frame)
        except cobs.DecodeError:
            self._logger.debug(f'Failed decode ({len(frame)} bytes)')
            self._logger.debug(frame)
            return

        if len(decoded) < 4:
            return

        payload = decoded[:-4]
        expected_crc = zlib.crc32(payload) & 0xffffffff
        provided_crc = (decoded[-1] << 24) \
                        | (decoded[-2] << 16) \
                        | (decoded[-3] << 8) \
                        | decoded[-4]
        
        if expected_crc != provided_crc:
            self._logger.debug(f'Bad CRC. expected={hex(expected_crc)}, actual={hex(provided_crc)}')
            return

        message = splitflap_pb2.FromSplitflap()
        message.ParseFromString(payload)

        # If this is an ack, notify the write thread
        if message.WhichOneof("payload") == 'ack':
            nonce = message.ack.nonce
            self._ack_q.put(nonce)

        self._logger.debug(message)
        with self._lock:
            if self._message_handler is not None:
                try:
                    self._message_handler(message)
                except:
                    self._logger.warning(f'Unhandled exception in message handler ({message.WhichOneof("payload")})', exec_info=True)
    
    def _write_loop(self):
        self._logger.debug('Write loop started')
        while True:
            data = self._out_q.get()
            # Check for shutdown
            if not self._run:
                self._logger.debug('Write loop exiting @ _out_q')
                return
            (nonce, encoded_message) = data

            next_retry = 0
            while True:
                if time.time() >= next_retry:
                    if next_retry > 0:
                        self._logger.debug('Retry write...')
                    self._serial.write(encoded_message)
                    self._serial.write(b'\0')
                    next_retry = time.time() + Splitflap.RETRY_TIMEOUT
                
                try:
                    latest_ack_nonce = self._ack_q.get(timeout=next_retry - time.time())
                except Empty:
                    latest_ack_nonce = None

                # Check for shutdown
                if not self._run:
                    self._logger.debug('Write loop exiting @ _ack_q')
                    return

                if latest_ack_nonce == nonce:
                    break
                else:
                    self._logger.debug(f'Got unexpected nonce: {latest_ack_nonce}')
    
    def _enqueue_message(self, message):
        nonce = self._next_nonce
        self._next_nonce += 1

        message.nonce = nonce

        payload = bytearray(message.SerializeToString())

        crc = zlib.crc32(payload) & 0xffffffff
        payload.append(crc & 0xff)
        payload.append((crc >> 8) & 0xff)
        payload.append((crc >> 16) & 0xff)
        payload.append((crc >> 24) & 0xff)

        encoded_message = cobs.encode(payload)

        self._out_q.put((nonce, encoded_message))
        self._logger.debug(f'Out q length: {self._out_q.qsize()}')

    def set_positions(self, indexes):
        message = splitflap_pb2.ToSplitflap()
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

        self._enqueue_message(message)

    def start(self):
        self.read_thread = Thread(target=self._read_loop)
        self.write_thread = Thread(target=self._write_loop)
        self.read_thread.start()
        self.write_thread.start()
    
    def shutdown(self):
        self._logger.debug('Shutting down')
        self._run = False
        self.read_thread.join()
        self._logger.debug('Read thread terminated')
        self._out_q.put(None)
        self._ack_q.put(None)
        self.write_thread.join()
        self._logger.debug('Write thread terminated')
    
    def set_handler(self, message_handler):
        with self._lock:
            self._message_handler = message_handler


@contextmanager
def splitflap_context(serial_port):
    with serial.Serial(serial_port, 230400, timeout=1.0) as ser:
        s = Splitflap(ser)
        s.start()
        try:
            yield s
        finally:
            s.shutdown()


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
    logging.basicConfig(level=logging.INFO, format='%(asctime)s:%(name)s:%(levelname)s:%(message)s')

    p = ask_for_serial_port()
    with splitflap_context(p) as s:
        logging.info('Connected')
        logging.info('Waiting for data...')
        q = Queue(1)
        def startup_handler(message):
            try:
                q.put_nowait(message)
            except Full:
                pass                
        s.set_handler(startup_handler)
        q.get()
        s.set_handler(None)

        while True:
            input()
            s.set_positions([1] * 108)
