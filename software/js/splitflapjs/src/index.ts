import SerialPort = require('serialport')
import {decode, encode} from 'cobs'
import * as CRC32 from 'crc-32'

import {PB} from './proto_gen/splitflap_proto.js'

export {PB}

export type MessageCallback = (message: PB.FromSplitflap) => void

interface QueueEntry {
    nonce: number
    encodedToSplitflapPayload: Uint8Array
}

export class Splitflap {
    private static readonly RETRY_MILLIS = 250

    private port: SerialPort | null
    private onMessage: MessageCallback
    private buffer: Buffer

    private outgoingQueue: QueueEntry[] = []

    private lastNonce: number = 1
    private retryTimeout: NodeJS.Timeout | null = null

    constructor(port: SerialPort | null, onMessage: MessageCallback) {
        this.port = port
        this.onMessage = onMessage

        this.buffer = Buffer.alloc(0)

        if (port !== null) {
            port.on('data', (data: Buffer) => {
                this.buffer = Buffer.concat([this.buffer, data])
                this.processBuffer()
            })
        }
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
        this.sendCommand(
            PB.SplitflapCommand.fromObject({
                modules,
            }),
        )
    }

    public resetModules(positions: boolean[]): void {
        const modules = positions.map((reset: boolean) => {
            return {
                action: reset
                    ? PB.SplitflapCommand.ModuleCommand.Action.RESET_AND_HOME
                    : PB.SplitflapCommand.ModuleCommand.Action.NO_OP,
            }
        })
        this.sendCommand(
            PB.SplitflapCommand.fromObject({
                modules,
            }),
        )
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
            const raw_buffer = this.buffer.slice(0, i)
            const packet = decode(raw_buffer) as Buffer
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
                console.debug(raw_buffer.toString())
                continue
            }

            let message: PB.FromSplitflap
            try {
                message = PB.FromSplitflap.decode(payload)
            } catch (err) {
                console.warn(`Invalid protobuf message ${payload}`)
                return
            }

            if (message.payload === 'ack') {
                this.handleAck(message.ack!.nonce!)
            }

            this.onMessage(message)
        }
    }

    private sendMessage(message: PB.ToSplitflap) {
        if (this.port === null) {
            return
        }
        message.nonce = this.lastNonce++

        // Encode before enqueueing to ensure messages don't change once they're queued
        const payload = PB.ToSplitflap.encode(message).finish()

        if (this.outgoingQueue.length > 10) {
            console.warn(`Splitflap outgoing queue overflowed! Dropping ${this.outgoingQueue.length} pending messages!`)
            this.outgoingQueue.length = 0
        }
        this.outgoingQueue.push({
            nonce: message.nonce,
            encodedToSplitflapPayload: payload,
        })
        this.serviceQueue()
    }

    private handleAck(nonce: number): void {
        if (this.outgoingQueue.length > 0 && nonce === this.outgoingQueue[0].nonce) {
            if (this.retryTimeout !== null) {
                clearTimeout(this.retryTimeout)
                this.retryTimeout = null
            }
            this.outgoingQueue.shift()
            this.serviceQueue()
        } else {
            console.debug(`Ignoring unexpected ack for nonce ${nonce}`)
        }
    }

    private serviceQueue(): void {
        if (this.retryTimeout !== null) {
            // Retry is pending; let the pending timeout handle the next step
            return
        }
        if (this.port === null) {
            return
        }
        if (this.outgoingQueue.length === 0) {
            return
        }

        const {encodedToSplitflapPayload: payload} = this.outgoingQueue[0]

        const crc = CRC32.buf(payload)
        const crcBuffer = Buffer.from([crc & 0xff, (crc >>> 8) & 0xff, (crc >>> 16) & 0xff, (crc >>> 24) & 0xff])
        const packet = Buffer.concat([payload, crcBuffer])

        const encodedDelimitedPacket = Buffer.concat([encode(packet), Buffer.from([0])])

        this.retryTimeout = setTimeout(() => {
            this.retryTimeout = null
            console.log(`Retrying ToSplitflap...`)
            this.serviceQueue()
        }, Splitflap.RETRY_MILLIS)

        console.log(`Sent ${payload.length} byte packet with CRC ${(crc >>> 0).toString(16)}`)
        this.port.write(encodedDelimitedPacket)
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
