import logging
import random
import time
import traceback
from threading import Thread

NUM_FLAPS = 40
FLAPS = [
    ' ',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '.',
    ',',
    '\'',
]
STEPS_PER_MOTOR_REVOLUTION = 32

# The gear ratio constants below represent the input:output ratio of the gearbox expressed as a simplified fraction.
# For example, for a gear train with ratios 31:10, 26:9, 22:11, 32:9, the overall ratio expressed as integers would be
# (31*26*22*32):(10*9*11*9) == 567424:8910 == 25792:405 ~= 63.684:1. To avoid floating point math, we would use the
# simplified integer fraction values 25792 and 405.
_GEAR_RATIO_INPUT = 64  # 25792
_GEAR_RATIO_OUTPUT = 1  # 405

# All motion is tracked in terms of motor steps (rather than motor revolutions), so we pre-multiply the gear ratio input
# by the number of motor steps per revolution as a more useful quantity to work with.
GEAR_RATIO_INPUT_STEPS = STEPS_PER_MOTOR_REVOLUTION * _GEAR_RATIO_INPUT

# Likewise, we care about the number of flaps flipped, rather than the number of output shaft revolutions, so we
# pre-multiply the gear ratio output by the number of flaps per revolution as a more useful quantity to work with.
GEAR_RATIO_OUTPUT_FLAPS = _GEAR_RATIO_OUTPUT * NUM_FLAPS

STATE_LOOK_FOR_HOME = 'LOOK_FOR_HOME'
STATE_PANIC = 'PANIC'
STATE_SENSOR_ERROR = 'SENSOR_ERROR'
STATE_NORMAL = 'NORMAL'

PERIOD_FROM_SPEED = [1000000, 20000, 7792, 6295, 5449, 4881, 4464, 4141, 3880, 3664, 3481, 3323, 3186, 3064, 2955, 2857, 2769, 2688, 2614, 2546, 2483, 2424, 2370, 2319, 2271, 2226, 2183, 2143, 2106, 2070, 2035, 2003, 1972, 1942, 1914, 1887, 1861, 1836, 1812, 1789, 1767, 1746, 1725, 1705, 1686, 1667, 1649, 1632, 1615, 1599, 1583, 1567, 1553, 1538, 1524, 1510, 1497, 1484, 1471, 1459, 1447, 1435, 1423, 1412, 1401, 1390, 1380, 1370, 1360, 1350, 1340, 1331, 1322, 1313, 1304, 1295, 1287, 1279, 1271, 1263, 1255, 1247, 1239, 1232, 1225, 1218, 1211, 1204]
MAX_SPEED = len(PERIOD_FROM_SPEED) - 1

# This is "rough" because it's integer division; it shouldn't be used for movement calculations or the error would
# accumulate.
_ROUGH_STEPS_PER_FLAP = GEAR_RATIO_INPUT_STEPS / GEAR_RATIO_OUTPUT_FLAPS

# The number of steps in either direction that's acceptable error for the home sensor
HOME_ERROR_MARGIN_STEPS = _ROUGH_STEPS_PER_FLAP / 4

# After finding the home position, how long to wait before considering another home blip to be an unexpected error
UNEXPECTED_HOME_START_BUFFER_STEPS = _ROUGH_STEPS_PER_FLAP * 5

# When recalibrating the home position, the number of steps to travel searching for home before giving up
MAX_STEPS_LOOKING_FOR_HOME = (NUM_FLAPS + 5) * _ROUGH_STEPS_PER_FLAP

# Ignore any home blips (e.g. if we've just seen the home position and haven't traveled past it yet)
HOME_STATE_IGNORE = 0
# Home isn't expected; a home blip in this state/region indicates an error that requires recalibration
HOME_STATE_UNEXPECTED = 1
# Home position is expected in this state/region
HOME_STATE_EXPECTED = 2


logger = logging.getLogger('main')


def micros():
    return int(time.time() * 1000000)


class Splitflap(object):

    def __init__(self):
        # Enable for auto-calibration via home sensor feedback. Disable for basic open-loop control.
        self.home_calibration_enabled = True

        self.last_update_micros = 0

        # Tracks the most recent target flap index. Not used during motion, but needed to recalculate target step if we
        # re-calibrate the home position
        self.target_flap_index = 0

        # Current position/destination. Numbers are modulo GEAR_RATIO_INPUT_STEPS
        self.current_step = 0
        self.delta_steps = 0

        # Home calibration state. All values recalculated whenever we see a home sensor blip
        self.home_state = HOME_STATE_IGNORE
        self.unexpected_home_start_step = 0  # Start of range where a home sensor blip is unexpected
        self.unexpected_home_end_step = 0  # End of range where a home sensor blip is unexpected

        # Expected home position step plus some margin of error. If we get to this step without having seen a home
        # sensor blip, something is wrong and we need to recalibrate.
        self.missed_home_step = 0

        self.steps_looking_for_home = 0

        # Motor state
        self.current_phase = 0
        self.current_speed = 0
        self.current_period = PERIOD_FROM_SPEED[0]

        self.state = STATE_LOOK_FOR_HOME if self.home_calibration_enabled else STATE_NORMAL

    def _get_flap_floor(self, step):
        return step * GEAR_RATIO_OUTPUT_FLAPS / GEAR_RATIO_INPUT_STEPS

    def go_to_flap(self, flap_char):
        if self.state != STATE_NORMAL or self.current_speed != 0:
            return

        self.target_flap_index = FLAPS.index(flap_char)
        self._go_to_target_flap_index()

    def _get_target_step_for_flap_index(self, from_step, target_flap_index):
        assert 0 <= from_step < 2*GEAR_RATIO_INPUT_STEPS
        from_flap = self._get_flap_floor(from_step)
        assert 0 <= from_flap < 2*NUM_FLAPS
        if from_flap >= NUM_FLAPS:
            from_flap_index = from_flap - NUM_FLAPS
        else:
            from_flap_index = from_flap

        if target_flap_index > from_flap_index:
            delta_flaps = target_flap_index - from_flap_index
        else:
            # Even if we're exactly at the target flap index, still do a full revolution to get to the target flap
            # since we're working with rounded numbers
            delta_flaps = NUM_FLAPS + target_flap_index - from_flap_index

        assert 0 < delta_flaps <= 40

        gear_ratio_input_steps_flaps_destination = (from_flap + delta_flaps) * GEAR_RATIO_INPUT_STEPS

        # Round UP when dividing so that the inverse calculation on the result (_get_flap_floor) returns the expected
        # result.
        result = gear_ratio_input_steps_flaps_destination / GEAR_RATIO_OUTPUT_FLAPS
        if gear_ratio_input_steps_flaps_destination % GEAR_RATIO_OUTPUT_FLAPS != 0:
            result += 1
        return result

    def _go_to_target_flap_index(self):
        self.delta_steps = self._get_target_step_for_flap_index(self.current_step, self.target_flap_index) - self.current_step

        logger.debug('Going to flap index {}. Current step is {}. Delta is {}.'.format(
            self.target_flap_index,
            self.current_step,
            self.delta_steps,
        ))

        assert self.delta_steps >= 0
        assert self.delta_steps <= GEAR_RATIO_INPUT_STEPS

    def go_home(self):
        if self.state == STATE_PANIC:
            return

        assert self.home_calibration_enabled
        self.state = STATE_LOOK_FOR_HOME
        self.steps_looking_for_home = 0

    def _update_expected_home(self):
        # Expected home position is the next 0 index flap position after the missed_home_step. This must be calculated
        # from the missed_home_step, rather than current_step, so that in the event of an early home, we don't compute
        # the next home as the one that is just a few steps away.

        expected_home = self._get_target_step_for_flap_index(self.missed_home_step, 0)

        new_unexpected_home_start_step = self.current_step + UNEXPECTED_HOME_START_BUFFER_STEPS
        new_unexpected_home_end_step = expected_home - HOME_ERROR_MARGIN_STEPS
        new_missed_home_step = expected_home + HOME_ERROR_MARGIN_STEPS

        logger.debug('Calculated new expected home {}.\nOLD:us={}, ue={}, m={}\nNEW:us={}, ue={}, m={}'.format(
            expected_home,
            self.unexpected_home_start_step,
            self.unexpected_home_end_step,
            self.missed_home_step,
            new_unexpected_home_start_step,
            new_unexpected_home_end_step,
            new_missed_home_step,
        ))

        # Values shouldn't be more than 2*GEAR_RATIO_INPUT_STEPS, so use subtraction to bound to GEAR_RATIO_INPUT_STEPS
        # rather than using `%` which may be more expensive
        assert 0 <= new_unexpected_home_start_step < 2*GEAR_RATIO_INPUT_STEPS
        assert 0 <= new_unexpected_home_end_step < 2*GEAR_RATIO_INPUT_STEPS
        assert 0 <= new_missed_home_step < 2*GEAR_RATIO_INPUT_STEPS
        if new_unexpected_home_start_step >= GEAR_RATIO_INPUT_STEPS:
            new_unexpected_home_start_step -= GEAR_RATIO_INPUT_STEPS
        if new_unexpected_home_end_step >= GEAR_RATIO_INPUT_STEPS:
            new_unexpected_home_end_step -= GEAR_RATIO_INPUT_STEPS
        if new_missed_home_step >= GEAR_RATIO_INPUT_STEPS:
            new_missed_home_step -= GEAR_RATIO_INPUT_STEPS

        # The "unexpected" range should never wrap around, since GEAR_RATIO_INPUT_STEPS represents an integer number of
        # FULL revolutions.
        assert new_unexpected_home_end_step > new_unexpected_home_start_step

        self.unexpected_home_start_step = new_unexpected_home_start_step
        self.unexpected_home_end_step = new_unexpected_home_end_step
        self.missed_home_step = new_missed_home_step
        self.home_state = HOME_STATE_IGNORE

    def update(self):
        now = micros()
        delta_time = now - self.last_update_micros
        if delta_time >= self.current_period:
            self.last_update_micros = now

            if self.state == STATE_NORMAL:
                reset_to_home = False
                if self.home_calibration_enabled:
                    found_home = self.check_home()
                    if self.home_state == HOME_STATE_IGNORE:
                        if found_home:
                            logger.debug('Ignoring HOME')
                        if self.current_step == self.unexpected_home_start_step:
                            self.home_state = HOME_STATE_UNEXPECTED
                    elif self.home_state == HOME_STATE_UNEXPECTED:
                        if found_home:
                            logger.warn('Unexpected home! At {}. Unexpected range {}-{}; missed at {}.'.format(
                                self.current_step,
                                self.unexpected_home_start_step,
                                self.unexpected_home_end_step,
                                self.missed_home_step,
                            ))
                            reset_to_home = True
                        elif self.current_step == self.unexpected_home_end_step:
                            self.home_state = HOME_STATE_EXPECTED
                    elif self.home_state == HOME_STATE_EXPECTED:
                        if found_home:
                            logger.debug('Found expected home.')
                            self._update_expected_home()
                        elif self.current_step == self.missed_home_step:
                            logger.warn('Missed expected home! At {}. Expected between {} and {}.'.format(
                                self.current_step,
                                self.unexpected_home_end_step,
                                self.missed_home_step,
                            ))
                            reset_to_home = True

                if reset_to_home:
                    self.go_home()
                    target_speed = 0
                else:
                    # Update speed based on distance to target
                    if self.delta_steps > MAX_SPEED:
                        target_speed = MAX_SPEED
                    else:
                        target_speed = self.delta_steps
            elif self.state == STATE_LOOK_FOR_HOME:
                assert self.home_calibration_enabled
                self.steps_looking_for_home += 1
                found_home = self.check_home()
                if found_home:
                    logger.info('Found home!')
                    self.state = STATE_NORMAL
                    target_speed = 0

                    # Reset frame of reference
                    self.current_step = 0
                    self.unexpected_home_start_step = 0
                    self.unexpected_home_end_step = 0
                    self.missed_home_step = 0
                    self._update_expected_home()

                    self._go_to_target_flap_index()
                else:
                    if self.steps_looking_for_home >= MAX_STEPS_LOOKING_FOR_HOME:
                        logger.info('Gave up looking for home!')
                        self.state = STATE_SENSOR_ERROR
                        target_speed = 0
                    else:
                        target_speed = 5
            else:
                target_speed = 0

            # Update motor
            if self.current_speed < target_speed:
                self.current_speed += 1
            elif self.current_speed > target_speed:
                self.current_speed -= 1

            self.current_period = PERIOD_FROM_SPEED[self.current_speed]

            if self.current_speed > 0:
                self.current_step = (self.current_step + 1) % GEAR_RATIO_INPUT_STEPS
                self.current_phase = (self.current_phase + 1) % 4
                if self.delta_steps > 0:
                    self.delta_steps -= 1
            # self.set_motor()

            logger.info('update: cs={}, delta={}, cp={}, us={}, ue={}, mh={}, speed={}, period={}, [flap {!r}]'.format(
                self.current_step,
                self.delta_steps,
                self.current_phase,
                self.unexpected_home_start_step,
                self.unexpected_home_end_step,
                self.missed_home_step,
                self.current_speed,
                self.current_period,
                FLAPS[self._get_flap_floor(self.current_step) % NUM_FLAPS],
            ))

            # Check modular arithmetic invariant
            assert self.current_step < GEAR_RATIO_INPUT_STEPS
            assert self.current_step >= 0

    def check_home(self):
        # TODO: fake this
        result = None
        if self.state == STATE_LOOK_FOR_HOME:
            result = random.random() < (1.5 / (NUM_FLAPS * _ROUGH_STEPS_PER_FLAP))
        elif self.state == STATE_NORMAL:
            if self.home_state == HOME_STATE_UNEXPECTED:
                result = random.random() < 0.00005
            elif self.home_state == HOME_STATE_EXPECTED:
                result = random.random() < 0.1

        if result is None:
            result = random.random() < 0.001

        if result:
            logger.debug('HOME!')
        return result


def run():
    logging.basicConfig(level=logging.DEBUG)

    splitflap = Splitflap()

    def run_thread():
        # noinspection PyBroadException
        try:
            while True:
                splitflap.update()
        except:
            logger.fatal('Exception:\n{}'.format(traceback.format_exc()))

    t = Thread(target=run_thread)
    t.setDaemon(True)
    t.start()
    while True:
        i = raw_input(">")
        if i == '@':
            splitflap.go_home()
        else:
            splitflap.go_to_flap(i)


if __name__ == '__main__':
    run()




