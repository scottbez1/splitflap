import SerialPort = require('serialport')
import { Splitflap, Util } from 'splitflapjs'

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

    const splitflap = new Splitflap(port, (message) => {
        if (message.payload === 'log') {
            console.log(message.log!.msg)
        }
    })

    const example = [
        [ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18],
        [19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36],
        [37, 38, 39,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14],
        [15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32],
        [33, 34, 35, 36, 37, 38, 39,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10],
        [11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28],
    ]

    const example2 = [
        [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
        [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
        [false, false, false, false, true, true, true, true, true, true, true, true, true, true, false, false, false, false],
        [false, false, false, false, true, true, true, true, true, true, true, true, true, true, false, false, false, false],
        [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
        [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
    ]

    setInterval(() => {
        splitflap.setPositions(Util.mapDualRowZigZagToLinear(example, true))
        setTimeout(() => {
            splitflap.resetModules(Util.mapDualRowZigZagToLinear(example2, true))
        }, 6000)
    }, 12000)
}

main()
