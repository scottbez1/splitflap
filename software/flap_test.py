from time import sleep

from splitflap import splitflap
from splitflap import ask_for_serial_port


def run():
    port = ask_for_serial_port()

    print('Starting...')
    with splitflap(port) as s:
        characters = s.get_character_list()

        while(True):
            for i, c in enumerate(characters):
                string = c * s.get_num_modules()
                print('Going to index {} (\'{}\')'.format(i, c))
                status = s.set_text(string)
                s.print_status(status)
                sleep(1)
            sleep(10)


if __name__ == '__main__':
    run()
