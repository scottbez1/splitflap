from splitflap import splitflap
from splitflap import ask_for_serial_port


def filter_string(string, sf):
    str_list = list(string)  # use list instead of string so it's mutable

    for i, c in enumerate(str_list):
        new_char = ' '  # replace with space if not in list
        if sf.in_character_list(c):
            continue  # in list, continue
        elif c.isalpha():
            alt_char = c.lower() if c.isupper() else c.upper()  # check for upper/lowercase equivalent
            if sf.in_character_list(alt_char):
                new_char = alt_char

        str_list[i] = new_char  # replace character in list

    return ''.join(str_list)


def run():
    port = ask_for_serial_port()

    print('Starting...')
    with splitflap(port) as s:
        while True:
            string = input("Enter text: ")
            if string == "exit" or string == "quit" or string == "q":
                break
            string = filter_string(string, s)  # filter characters to displayable ones
            string = string.ljust(s.get_num_modules(), ' ')  # pad to the right with spaces
            print('Going to \'{}\''.format(string))
            status = s.set_text(string)
            s.print_status(status)


if __name__ == '__main__':
    run()
