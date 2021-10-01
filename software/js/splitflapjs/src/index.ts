import SerialPort = require('serialport')
import {decode, encode} from 'cobs'
import * as CRC32 from 'crc-32'

import {PB} from './proto_gen/splitflap_proto.js'

export { PB }

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

    public setPositions(positions: Array<number | null>): void {
        const modules = positions.map((position: number | null) => {
            if (position === null) {
                return {
                    action: PB.SplitflapCommand.ModuleCommand.Action.NO_OP,
                }
            } else {
                return {
                    action: PB.SplitflapCommand.ModuleCommand.Action.GO_TO_FLAP,
                    param: position,
                }
            }
        })
        this.sendCommand(PB.SplitflapCommand.fromObject({
            modules
        }))
    }

    public resetModules(positions: boolean[]): void {
        const modules = positions.map((reset: boolean) => {
            return {
                action: reset ? PB.SplitflapCommand.ModuleCommand.Action.RESET_AND_HOME : PB.SplitflapCommand.ModuleCommand.Action.NO_OP,
            }
        })
        this.sendCommand(PB.SplitflapCommand.fromObject({
            modules
        }))
    }

    public resetModule(position: number): void {
        const reset = new Array(position + 1)
        reset.fill(false)
        reset[position] = true
        this.resetModules(reset)
    }

    private sendCommand(command: PB.SplitflapCommand): void {
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
