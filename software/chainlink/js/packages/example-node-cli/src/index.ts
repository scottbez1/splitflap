import SerialPort = require('serialport')

import readline from 'readline'

import {SplitflapNode} from 'splitflapjs-node'
import {PB} from 'splitflapjs-proto'
import { applySetFlaps } from 'splitflapjs-core/dist/util'

// Edit this to restrict to a single device based on serial number, e.g. add something like '02280A9E' to this array.
// If this is left blank, a serial device matching the vendor/product codes from SplitflapNode.USB_DEVICE_FILTERS
// will be selected.
const USB_SERIAL_NUMBERS: Array<string> = []

// Make sure this matches config.h in the splitflap firmware
const FLAPS = [
    ' ',
    'a',
    'b',
    'c',
    'd',
    'e',
    'f',
    'g',
    'h',
    'i',
    'j',
    'k',
    'l',
    'm',
    'n',
    'o',
    'p',
    'q',
    'r',
    's',
    't',
    'u',
    'v',
    'w',
    'x',
    'y',
    'z',
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '.',
    ',',
    "'",
]

const main = async (): Promise<void> => {
    const ports = await SerialPort.list()

    const matchingPorts = ports.filter((portInfo) => {
        return USB_SERIAL_NUMBERS.length > 0
            ? portInfo.serialNumber !== undefined && USB_SERIAL_NUMBERS.includes(portInfo.serialNumber)
            : SplitflapNode.USB_DEVICE_FILTERS.some(
                  (f) =>
                      f.usbVendorId.toString(16) === portInfo.vendorId &&
                      f.usbProductId.toString(16) === portInfo.productId,
              )
    })

    if (matchingPorts.length < 1) {
        console.error(`No splitflap usb serial port found! ${JSON.stringify(ports, undefined, 4)}`)
        return
    } else if (matchingPorts.length > 1) {
        console.error(
            `Multiple possible splitflap usb serial ports found: ${JSON.stringify(matchingPorts, undefined, 4)}`,
        )
        // TODO: offer option to select one via input rather than failing hard
        return
    }

    const portInfo = matchingPorts[0]
    let splitflapConfig = PB.SplitflapConfig.create()
    let onSplitflapStateReceived: () => void
    const splitflapStateReceived = new Promise((resolve) => onSplitflapStateReceived = () => resolve(null));
    const splitflap = new SplitflapNode(
        portInfo.path,
        (message: PB.FromSplitflap) => {
            if (message.payload === 'log' && message.log) {
                console.log(message.log.msg)
            } else if (
                message.payload === 'splitflapState' &&
                message.splitflapState &&
                message.splitflapState.modules
            ) {
                console.log(
                    `State:\n${message.splitflapState.modules.map((mod) => {
                        return mod.flapIndex
                    })}`,
                )
                if (splitflapConfig.modules.length === 0) {
                    // First time we get a state report, initialize our config with the appropriate number of modules
                    for (let i = 0; i < message.splitflapState.modules.length; i++) {
                        splitflapConfig.modules.push(PB.SplitflapConfig.ModuleConfig.create({
                            targetFlapIndex: 0
                        }))
                    }
                    onSplitflapStateReceived()
                } else {
                    if (splitflapConfig.modules.length !== message.splitflapState.modules.length) {
                        console.warn('Number of modules in state is different than number of modules in config', {
                            nModulesState: message.splitflapState.modules.length,
                            nModulesConfig: splitflapConfig.modules.length,
                        })
                    }
                }
            } else if (message.payload === 'supervisorState' && message.supervisorState) {
                console.log(
                    `Supervisor state:\n${JSON.stringify(
                        PB.SupervisorState.toObject(message.supervisorState as PB.SupervisorState, {defaults: true}),
                        undefined,
                        4,
                    )}`,
                )
            }
        },
    )

    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout,
    })
    const reset = await new Promise<string>((resolve) => {
        rl.question('Reset? y/n', resolve)
    })
    if (reset === 'y') {
        await splitflap.hardReset()
    }

    // TODO: make this wait for idle instead of just any state
    console.log('Waiting to hear from Splitflap...')
    await splitflapStateReceived

    type anim = [number, string]
    const animation: anim[] = [
        [6000, 'hello'],
        [15000, 'world'],
    ]

    let cur = 0
    const runAnimation = () => {
        splitflapConfig = applySetFlaps(splitflapConfig, stringToFlapIndexArray(animation[cur][1]))
        splitflap.sendConfig(splitflapConfig)
        setTimeout(runAnimation, animation[cur][0])
        cur = (cur + 1) % animation.length
    }

    runAnimation()
}

const charToFlapIndex = (c: string): number | null => {
    const i = FLAPS.indexOf(c)
    if (i >= 0) {
        return i
    } else {
        return null
    }
}

const stringToFlapIndexArray = (str: string): Array<number | null> => {
    return str.split('').map(charToFlapIndex)
}

main()
