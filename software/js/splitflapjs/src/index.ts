import SerialPort = require('serialport')
import {decode, encode} from 'cobs'
import * as CRC32 from 'crc-32'

import {PB} from 'splitflapjs-proto'

export type MessageCallback = (message: PB.FromSplitflap) => void

interface QueueEntry {
    nonce: number
    encodedToSplitflapPayload: Uint8Array
}


const sleep = (millis: number) => {
    return new Promise((resolve) => { setTimeout(resolve, millis) })
}

export class Splitflap {
    private static readonly RETRY_MILLIS = 250
    private static readonly BAUD = 230400

    private port: SerialPort | null
    private onMessage: MessageCallback
    private buffer: Buffer
    private numModules: number

    private outgoingQueue: QueueEntry[] = []

    private lastNonce: number = 1
    private retryTimeout: NodeJS.Timeout | null = null

    private currentConfig: PB.SplitflapConfig

    constructor(serialPath: string | null, onMessage: MessageCallback, numModules: number) {
        this.onMessage = onMessage

        this.buffer = Buffer.alloc(0)
        this.numModules = numModules

        if (serialPath !== null) {
            this.port = new SerialPort(serialPath, {
                baudRate: Splitflap.BAUD,
            })
            this.port.on('data', (data: Buffer) => {
                this.buffer = Buffer.concat([this.buffer, data])
                this.processBuffer()
            })
        } else {
            this.port = null
        }

        this.currentConfig = PB.SplitflapConfig.create()
        for (let i = 0; i < numModules; i++) {
            this.currentConfig.modules.push(PB.SplitflapConfig.ModuleConfig.create())
        }
    }

    public setFlaps(positions: Array<number | null>, forceMovement?: Array<boolean>): void {
        if (positions.length > this.numModules) {
            throw new Error(`More positions specified (${positions.length}) than modules (${this.numModules})!`)
        }
        if (forceMovement !== undefined && forceMovement.length != positions.length) {
            throw new Error(`forceMovement (${forceMovement.length}) and positions (${positions.length}) length mismatch!`)
        }

        for (let i = 0; i < positions.length; i++) {
            if (positions[i] !== null) {
                this.currentConfig.modules[i].targetFlapIndex = positions[i]
            }
            if (forceMovement !== undefined && forceMovement[i]) {
                this.currentConfig.modules[i].movementNonce = (this.currentConfig.modules[i].movementNonce || 0) + 1
            }
        }
        this.sendConfig(this.currentConfig)
    }

    public resetModules(positions: boolean[]): void {
        if (positions.length > this.numModules) {
            throw new Error(`More positions specified (${positions.length}) than modules (${this.numModules})!`)
        }
        for (let i = 0; i < positions.length; i++) {
            this.currentConfig.modules[i].resetNonce = (this.currentConfig.modules[i].resetNonce || 0) + 1
        }
        this.sendConfig(this.currentConfig)
    }

    public resetModule(position: number): void {
        const reset = new Array(position + 1)
        reset.fill(false)
        reset[position] = true
        this.resetModules(reset)
    }

    /**
     * Perform a hard reset of the splitflap MCU. Takes a few seconds.
     */
    public async hardReset(): Promise<void> {
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

    private sendConfig(config: PB.SplitflapConfig): void {
        this.sendMessage(
            PB.ToSplitflap.create({
                splitflapConfig: config,
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
    export function convert2dDualRowZigZagTo1dChainlink<T>(arr: T[][], flipFirstRows: boolean = false): T[] {
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

    export function convert1dChainlinkTo2dDualRowZigZag<T>(arr: T[], cols: number, flipFirstRows: boolean = false): T[][] {
        const rows = Math.ceil(arr.length / cols)

        const out: T[][] = new Array<T[]>(rows)
        for (let i = 0; i < rows; i++) {
            out[i] = new Array<T>(cols)
        }

        for (let i = 0; i < arr.length; i++) {
            const rowPair = Math.floor(Math.floor(i / 2) / cols)
            const upsideDownRowPair = (rowPair + (flipFirstRows ? 1 : 0)) % 2 !== 0

            const rowBase = rowPair * 2
            const rowOffset = upsideDownRowPair ? i % 2 : 1 - (i % 2)

            const row = rowBase + rowOffset
            const col = upsideDownRowPair ? cols - 1 - (Math.floor(i / 2) % cols) : Math.floor(i / 2) % cols

            out[row][col] = arr[i]
        }
        return out
    }
}
