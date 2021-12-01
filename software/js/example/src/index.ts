import SerialPort = require('serialport')
import {Splitflap, Util} from 'splitflapjs'
import {PB} from 'splitflapjs-proto'

const main = async (): Promise<void> => {
    const ports = await SerialPort.list()

    const matchingPorts = ports.filter((portInfo) => {
        // Implement a check for your device's vendor+product+serial
        // (this is more robust than the alternative of just hardcoding a "path" like "/dev/ttyUSB0")
        return portInfo.vendorId === '10c4' && portInfo.productId === 'ea60' && (
            portInfo.serialNumber === '02280A9E' || portInfo.serialNumber === '022809A3')
    })

    if (matchingPorts.length < 1) {
        console.error(`No splitflap usb serial port found! ${JSON.stringify(ports, undefined, 4)}`)
        return
    } else if (matchingPorts.length > 1) {
        console.error(`Multiple splitflap usb serial ports found: ${JSON.stringify(matchingPorts, undefined, 4)}`)
        return
    }

    const portInfo = matchingPorts[0]
    const splitflap = new Splitflap(portInfo.path, (message: PB.FromSplitflap) => {
        if (message.payload === 'log') {
            console.log(message.log!.msg)
        } else if (message.payload === 'splitflapState' && message.splitflapState && message.splitflapState.modules) {
            // const remapped = Util.convert1dChainlinkTo2dDualRowZigZag(message.splitflapState.modules, 18, true)
            // let s = ''
            // for (let i = 0; i < remapped.length; i++) {
            //     s += JSON.stringify(remapped[i].map((mod) => { return mod.flapIndex })) + '\n'
            // }
            console.log(`State:\n${message.splitflapState.modules.map((mod) => { return mod.flapIndex })}`)
        } else if (message.payload === 'supervisorState' && message.supervisorState) {
            console.log(`Supervisor state:\n${JSON.stringify(PB.SupervisorState.toObject(message.supervisorState as PB.SupervisorState, {defaults: true}), undefined, 4)}`)
        }
    }, 108)

    const readline = require("readline")
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    })

    const reset = await new Promise<string>((resolve) => {
        rl.question("Reset? y/n", resolve);
    })

    if (reset === 'y') {
        await splitflap.hardReset()
    }

    // Make sure this matches config.h in the splitflap firmware
    const flaps = [
        ' ',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '.',
        ',',
        '\'',
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

    type anim = [number, string]
    const animation: anim[] = [
        [8000, 'hello'],
        [8000, 'world'],
    ]

    let cur = 0
    const runAnimation = () => {
        splitflap.setFlaps(stringToFlapIndexArray(animation[cur][1]))
        setTimeout(runAnimation, animation[cur][0])
        cur = (cur + 1) % animation.length
    }

    runAnimation()
}

main()
