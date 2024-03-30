import SerialPort = require('serialport')
import {MessageCallback, SplitflapCore} from 'splitflapjs-core'

const sleep = (millis: number) => {
    return new Promise((resolve) => { setTimeout(resolve, millis) })
}

export class SplitflapNode extends SplitflapCore {
    private port: SerialPort | null

    constructor(serialPath: string, onMessage: MessageCallback) {
        super(onMessage, (packet: Uint8Array) => {
            this.port?.write(Buffer.from(packet))
        })
        this.port = new SerialPort(serialPath, {
            baudRate: SplitflapCore.BAUD,
        })
        this.port.on('data', (data: Uint8Array) => {
            this.onReceivedData(data)
        })
        this.portAvailable = true
        this.onStart();
    }

    public override async hardReset() {
        if (this.port === null) {
            console.warn('Not connected to splitflap, so hard reset isn\'t possible')
            return
        }

        this.port.set({rts: true, dtr: false})
        await sleep(200)
        this.port.set({rts: true, dtr: true})
        await sleep(200)
        return
    }
}
