import SerialPort = require('serialport')
import { Splitflap, Util, PB } from 'splitflapjs'

const main = async (): Promise<void> => {
    const ports = await SerialPort.list()

    const matchingPorts = ports.filter((portInfo) => {
        return portInfo.vendorId === '10c4' &&
                portInfo.productId === 'ea60' &&
                portInfo.serialNumber === '02280A9E'
    })

    if (matchingPorts.length < 1) {
        console.error(`No splitflap usb serial port found!`)
        return
    } else if (matchingPorts.length > 1) {
        console.error(`Multiple splitflap usb serial ports found: ${JSON.stringify(matchingPorts, undefined, 4)}`)
        return
    }

    const portInfo = matchingPorts[0]
    const port = new SerialPort(portInfo.path, {
        baudRate: 230400,
    })

    const splitflap = new Splitflap(port, (message: PB.FromSplitflap) => {
        if (message.payload === 'log') {
            console.log(message.log!.msg)
        }
    })

    const flaps = [ 
        ' ',      // BLACK
        'J',    // 1
        'B',    // 2
        'M',    // 3
        'R',    // 4
        '$',    // 5
        'V',    // 6
        'K',    // 7
        'A',    // 8
        'E',    // 9
        'N',    // 10
        'O',    // 11
        'y',     // YELLOW
        '*',    // 13
        'g',     // GREEN
        'G',    // 15
        'I',    // 16
        '%',    // 17
        'D',    // 18
        'L',    // 19
        '&',    // 20
        '@',    // 21
        'C',    // 22
        'W',    // 23
        'H',    // 24
        'Y',    // 25
        'w',     // WHITE
        'Q',    // 27
        'p',     // PINK
        'o',     // ORANGE
        '!',    // 30
        'T',    // 31
        'Z',    // 32
        'P',    // 33
        'F',    // 34
        '?',    // 35
        'S',    // 36
        '#',    // 37
        'U',    // 38
        'X'     // 39
    ]

    const charToFlapIndex = (c: string): number | null => {
        const i = flaps.indexOf(c)
        if (i >= 0) {
            return i
        } else {
            return null
        }
    }

    const stringToFlapIndexArray = (str: string): Array<number | null> => {
        return str.split('').map(charToFlapIndex)
    }

    const example1 = [
        [ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18],
        [19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36],
        [37, 38, 39,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14],
        [15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32],
        [33, 34, 35, 36, 37, 38, 39,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10],
        [11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28],
    ]

    const example2 = [
        '~~~~~~~~~~~~~~~~~~',
        '~~~            ~~~',
        '~~~            ~~~',
        '~~~            ~~~',
        '~~~            ~~~',
        '~~~~~~~~~~~~~~~~~~',
    ]

    const example3 = [
        '~~~~~~~~~~~~~~~~~~',
        '~~~~~~~~~~~~~~~~~~',
        '~~~~HELLO~~~~~~~~~',
        '~~~~~~~~~WORLD~~~~',
        '~~~~~~~~~~~~~~~~~~',
        '~~~~~~~~~~~~~~~~~~',
    ]
    
    const go1 = () => {
        splitflap.setPositions(Util.mapDualRowZigZagToLinear(example1, true))
        setTimeout(go2, 6000)
    }

    const go2 = () => {
        splitflap.setPositions(Util.mapDualRowZigZagToLinear(example2.map(stringToFlapIndexArray), true))
        setTimeout(go3, 6000)
    }

    const go3 = () => {
        splitflap.setPositions(Util.mapDualRowZigZagToLinear(example3.map(stringToFlapIndexArray), true))
        setTimeout(go1, 6000)
    }

    go1()
}

main()
