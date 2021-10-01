import SerialPort = require('serialport')
import {decode, encode} from 'cobs'
import * as CRC32 from 'crc-32'

import {PB} from './proto_gen/splitflap_proto.js'

export type MessageCallback = (message: PB.FromSplitflap) => void

export class Splitflap {
    private port: SerialPort
    private onMessage: MessageCallback
    private buffer: Buffer
    constructor(port: SerialPort, onMessage: MessageCallback) {
        this.port = port
        this.onMessage = onMessage

        this.buffer = Buffer.alloc(0)

        port.on('data', (data: Buffer) => {
            this.buffer = Buffer.concat([this.buffer, data])
            this.processBuffer()
        })
    }

    public sendCommand(command: PB.SplitflapCommand): void {
        this.sendMessage(
            PB.ToSplitflap.create({
                splitflapCommand: command,
            }),
        )
    }

    private processBuffer(): void {
        var i: number
        // Iterate 0-delimited packets
        while ((i = this.buffer.indexOf(0)) != -1) {
            const packet = decode(this.buffer.slice(0, i)) as Buffer
            this.buffer = this.buffer.slice(i + 1)
            if (packet.length <= 4) {
                console.debug(`Received short packet ${this.buffer.slice(0, i)}`)
                continue
            }
            const payload = packet.slice(0, packet.length - 4)

            // Validate CRC32
            const crc_buf = packet.slice(packet.length - 4, packet.length)
            const provided_crc = crc_buf[0] | (crc_buf[1] << 8) | (crc_buf[2] << 16) | (crc_buf[3] << 24)
            const crc = CRC32.buf(payload)
            if (crc !== provided_crc) {
                console.debug(`Bad CRC. Expected ${crc} but received ${provided_crc}`)
                continue
            }

            let message: PB.FromSplitflap
            try {
                message = PB.FromSplitflap.decode(payload)
            } catch (err) {
                console.warn(`Invalid protobuf message ${payload}`)
                return
            }
            this.onMessage(message)
        }
    }

    private sendMessage(message: PB.ToSplitflap) {
        const payload = PB.ToSplitflap.encode(message).finish()
        const crc = CRC32.buf(payload)
        const crcBuffer = Buffer.from([crc & 0xff, (crc >>> 8) & 0xff, (crc >>> 16) & 0xff, (crc >>> 24) & 0xff])
        const packet = Buffer.concat([payload, crcBuffer])

        const encodedDelimitedPacket = Buffer.concat([encode(packet), Buffer.from([0])])
        this.port.write(encodedDelimitedPacket)
        console.log(`Sent ${payload.length} byte packet with CRC ${(crc >>> 0).toString(16)}`)
    }
}

export namespace Util {
    export function mapDualRowZigZagToLinear<T>(arr: T[][], flipFirstRows: boolean = false): T[] {
        const cols = arr[0].length
        const rows = arr.length

        const out: T[] = new Array<T>(cols * rows)
        for (let i = 0; i < cols * rows; i++) {
            const rowPair = Math.floor(Math.floor(i / 2) / cols)
            const upsideDownRowPair = (rowPair + (flipFirstRows ? 1 : 0)) % 2 !== 0

            const rowBase = rowPair * 2
            const rowOffset = upsideDownRowPair ? i % 2 : 1 - (i % 2)

            const row = rowBase + rowOffset
            const col = upsideDownRowPair ? cols - 1 - (Math.floor(i / 2) % cols) : Math.floor(i / 2) % cols

            out[i] = arr[row][col]
        }
        return out
    }
}

const main = async (): Promise<void> => {
    // const ports = await SerialPort.list()
    // console.log(ports)
    const port = new SerialPort('/dev/ttyUSB0', {
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
    const moduleData = Util.mapDualRowZigZagToLinear(example, true).map((v) => {
        return {
            action: PB.SplitflapCommand.ModuleCommand.Action.GO_TO_FLAP,
            param: v,
        }
    })

    setInterval(() => {
        splitflap.sendCommand(
            PB.SplitflapCommand.fromObject({
                modules: moduleData
            }),
        )
    }, 6000)
}

main()
