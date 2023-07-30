import random
import time

from splitflap_proto import (
    ask_for_serial_port,
    splitflap_context,
)

words = [
    'alpaca', 'baboon', 'badger', 'beluga', 'bobcat', 'ferret',
    'gopher', 'impala', 'jackal', 'jaguar', 'kitten', 'marmot',
    'monkey', 'ocelot', 'rabbit', 'racoon', 'turtle', 'walrus',
    'weasel', 'wombat',
]


def _run():
    p = ask_for_serial_port()
    with splitflap_context(p) as s:
        modules = s.get_num_modules()
        alphabet = s.get_alphabet()

        # Show a random word every 10 seconds
        while True:
            word = random.choice(words)
            s.set_text(word)
            time.sleep(10)


if __name__ == '__main__':
    _run()
