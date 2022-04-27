import argparse
from cobs import cobs
from collections import (
    defaultdict,
)
from contextlib import contextmanager
from enum import Enum
import logging
import os
from queue import (
    Empty,
    Full,
    Queue,
)
from random import randint
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

SPLITFLAP_BAUD = 230400


class Splitflap(object):

    class ForceMovement(Enum):
        NONE = 1            # No movement if module is already at the specified character
        ONLY_NON_BLANK = 2  # Force a full revolution if module is already at the specified character, but only if it's not the blank (' ') character
        ALL = 3             # All modules move on every command, regardless of whether they are already at the specified character

    RETRY_TIMEOUT = 0.25

    # TODO: read alphabet from splitflap once this is possible
    _DEFAULT_ALPHABET = [
        ' ',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '.',
        ',',
        '\'',
    ]

    def __init__(self, serial_instance):
        self._serial = serial_instance
        self._logger = logging.getLogger('splitflap')
        self._out_q = Queue()
        self._ack_q = Queue()
        self._next_nonce = randint(0, 255)
        self._run = True

        self._lock = Lock()
        self._message_handlers = defaultdict(list)

        self._current_config = splitflap_pb2.SplitflapConfig()
        self._num_modules = None

        self._alphabet = Splitflap._DEFAULT_ALPHABET

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
        self._logger.debug(message)

        payload_type = message.WhichOneof('payload')

        # If this is an ack, notify the write thread
        if payload_type == 'ack':
            nonce = message.ack.nonce
            self._ack_q.put(nonce)
        elif payload_type == 'splitflap_state':
            num_modules_reported = len(message.splitflap_state.modules)
            if self._num_modules is None:
                self._num_modules = num_modules_reported
                for i in range(num_modules_reported):
                    self._current_config.modules.append(splitflap_pb2.SplitflapConfig.ModuleConfig())
            else:
                assert self._num_modules == num_modules_reported, f'Number of reported modules changed (was {self._num_modules}, now {num_modules_reported})'

        with self._lock:
            for handler in self._message_handlers[payload_type] + self._message_handlers[None]:
                try:
                    handler(getattr(message, payload_type))
                except:
                    self._logger.warning(f'Unhandled exception in message handler ({payload_type})', exc_info=True)
    
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

        approx_q_length = self._out_q.qsize()
        self._logger.debug(f'Out q length: {approx_q_length}')
        if approx_q_length > 10:
            self._logger.warning(f'Output queue length is high! ({approx_q_length}) Is the splitflap still connected and functional?')

    def get_alphabet(self):
        return self._alphabet

    def set_text(self, text, force_movement=ForceMovement.NONE):
        """Helper for setting a string message. Using set_positions is preferable for more control."""

        # Transform text to a list of flap indexes (and pad with blanks so that all modules get updated even if text is shorter)
        positions = [self._alphabet.index(c) if c in self._alphabet else 0 for c in text] + [self._alphabet.index(' ')] * (self._num_modules - len(text))

        if force_movement == Splitflap.ForceMovement.NONE:
            force_movement = None
        elif force_movement == Splitflap.ForceMovement.ONLY_NON_BLANK:
            force_movement = [c in self._alphabet and c != ' ' for c in text] + [False] * (self._num_modules - len(text))
        elif force_movement == Splitflap.ForceMovement.ALL:
            force_movement = [True] * self._num_modules
        else:
            raise RuntimeError(f'bad value {force_movement}')

        self.set_positions(positions, force_movement)

    def set_positions(self, positions, force_movement=None):
        assert self._num_modules is not None, 'Cannot set positions before number of modules is known'

        assert len(positions) <= self._num_modules, 'More positions specified than modules'
        if force_movement is not None:
            assert len(positions) == len(force_movement), 'positions and force_movement list length must match'

        for i in range(len(positions)):
            if positions[i] is not None:
                self._current_config.modules[i].target_flap_index = positions[i]
            if force_movement is not None and force_movement[i]:
                self._current_config.modules[i].movement_nonce = (self._current_config.modules[i].movement_nonce + 1) % 256

        message = splitflap_pb2.ToSplitflap()
        message.splitflap_config.CopyFrom(self._current_config)
        self._enqueue_message(message)

    def start(self):
        self.read_thread = Thread(target=self._read_loop)
        self.write_thread = Thread(target=self._write_loop)
        self.read_thread.start()
        self.write_thread.start()
    
    def shutdown(self):
        self._logger.info('Shutting down...')
        self._run = False
        self.read_thread.join()
        self._logger.debug('Read thread terminated')
        self._out_q.put(None)
        self._ack_q.put(None)
        self.write_thread.join()
        self._logger.debug('Write thread terminated')
    
    def add_handler(self, message_type, handler):
        with self._lock:
            self._message_handlers[message_type].append(handler)
        return lambda: self._remove_handler(message_type, handler)

    def _remove_handler(self, message_type, handler):
        with self._lock:
            self._message_handlers[message_type].remove(handler)

    def request_state(self):
        message = splitflap_pb2.ToSplitflap()
        message.request_state.SetInParent()
        self._enqueue_message(message)

    def hard_reset(self):
        self._serial.setRTS(True)
        self._serial.setDTR(False)
        time.sleep(0.2)
        self._serial.setDTR(True)
        time.sleep(0.2)

    def get_num_modules(self):
        assert self._num_modules is not None, 'Num modules is unknown. Make sure to wait for a state message before querying'
        return self._num_modules


@contextmanager
def splitflap_context(serial_port, default_logging=True, wait_for_comms=True):
    with serial.Serial(serial_port, SPLITFLAP_BAUD, timeout=1.0) as ser:
        s = Splitflap(ser)
        s.start()

        if default_logging:
            s.add_handler('log', lambda msg: logging.info(f'From splitflap: {msg.msg}'))

        if wait_for_comms:
            logging.info('Connecting to splitflap...')
            q = Queue(1)
            def startup_handler(message):
                try:
                    q.put_nowait(None)
                except Full:
                    pass
            unregister = s.add_handler('splitflap_state', startup_handler)

            s.request_state()
            q.get()
            unregister()
            logging.info('Connected!')

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


def _run_example():
    p = ask_for_serial_port()
    with splitflap_context(p) as s:
        modules = s.get_num_modules()
        alphabet = s.get_alphabet()

        # Set up a handler to log reported state changes
        def state_str(s):
            if s == splitflap_pb2.SplitflapState.ModuleState.State.NORMAL:
                return '\u2713'
            elif s == splitflap_pb2.SplitflapState.ModuleState.State.LOOK_FOR_HOME:
                return '\u2370'
            elif s == splitflap_pb2.SplitflapState.ModuleState.State.SENSOR_ERROR:
                return '\u26a0'
            elif s == splitflap_pb2.SplitflapState.ModuleState.State.PANIC:
                return '\u2342'
            elif s == splitflap_pb2.SplitflapState.ModuleState.State.DISABLED:
                return '\u29bb'


        last_state_string = ''
        def log_state(message):
            nonlocal last_state_string
            state_string = ''.join([alphabet[m.flap_index] if m.state == splitflap_pb2.SplitflapState.ModuleState.State.NORMAL else state_str(m.state) for m in message.modules])
            if state_string != last_state_string:
                logging.info('State: ' + state_string)
                last_state_string = state_string
        s.add_handler('splitflap_state', log_state)
        s.request_state()

        # Loop forever
        while True:
            text = input()[:modules]

            s.set_text(text, Splitflap.ForceMovement.ALL)


if __name__ == '__main__':
    parser = argparse.ArgumentParser('Splitflap python interface example')
    parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose logging')
    args = parser.parse_args()

    log_level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=log_level, format='%(asctime)s:%(name)s:%(levelname)s:%(message)s')

    _run_example()
