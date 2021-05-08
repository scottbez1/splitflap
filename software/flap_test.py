from time import sleep

from splitflap import splitflap
from splitflap import ask_for_serial_port


def run():
    port = ask_for_serial_port()

    print('Starting...')
    with splitflap(port) as s:
        characters = s.get_character_list()
        num_flaps = len(characters)

        # make sure all modules are home'd before we continue
        while(True):
            at_home = True
            for module in s.get_status():
                at_home &= (module["flap"] == characters[0])
            if not at_home:
                s.recalibrate_all()
            else:
                break

        index = 1  # we're at home (#0), start at flap #1

        # loop through all flaps on every module
        while(True):
            char = characters[index]
            string = char * s.get_num_modules()
            print('Going to index {} (\'{}\')'.format(index, char))
            status = s.set_text(string)
            s.print_status(status)
            if index == 0:
                break  # we're back home
            index = (index + 1) % num_flaps
            sleep(1)


if __name__ == '__main__':
    run()
