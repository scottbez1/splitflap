import * as CRC32 from 'crc-32'
import { PB } from 'splitflapjs-proto'

import { decode as cobsDecode, encode as cobsEncode } from './cobs'
import { convert1dChainlinkTo2dDualRowZigZag, convert2dDualRowZigZagTo1dChainlink } from './util'

export type MessageCallback = (message: PB.FromSplitflap) => void
export type SendBytes = (packet: Uint8Array) => void

type QueueEntry = {
    nonce: number
    encodedToSplitflapPayload: Uint8Array
}

export { cobsDecode, cobsEncode, convert1dChainlinkTo2dDualRowZigZag, convert2dDualRowZigZagTo1dChainlink }

export class SplitflapCore {
    private static readonly RETRY_MILLIS = 250
    public static readonly BAUD = 230400
    public static readonly USB_DEVICE_FILTERS = [
        // 10c4:ea60 Silicon Labs CP210x UART Bridge
        {
            usbVendorId: 0x10c4,
            usbProductId: 0xea60,
        },
        // 1a86:55d4 QinHeng Electronics USB Single Serial
        {
            usbVendorId: 0x1a86,
            usbProductId: 0x55d4,
        },
    ]

    private onMessage: MessageCallback
    private sendBytes: SendBytes

    private readonly outgoingQueue: QueueEntry[] = []

    private lastNonce = 1
    private retryTimeout: ReturnType<typeof setTimeout> | null = null
    protected portAvailable = false

    private buffer = new Uint8Array()

    constructor(onMessage: MessageCallback, sendBytes: SendBytes) {
        this.lastNonce = Math.floor(Math.random() * (2 ^ (32 - 1)))
        this.onMessage = onMessage
        this.sendBytes = sendBytes
    }

    public sendConfig(config: PB.SplitflapConfig): void {
        this.enqueueMessage(
            PB.ToSplitflap.create({
                splitflapConfig: config,
            }),
        )
    }

    /**
     * Perform a hard reset of the splitflap MCU. May take a few seconds.
     */
    protected async hardReset(): Promise<void> {
        throw new Error("Not implemented!")
    }

    protected onStart() {
        this.enqueueMessage(PB.ToSplitflap.create({
            requestState: PB.RequestState.create(),
        }))
    }

    protected onReceivedData(data: Uint8Array) {
        this.buffer = Uint8Array.from([...this.buffer, ...data])

        let i: number
        // Iterate 0-delimited packets
        while ((i = this.buffer.indexOf(0)) != -1) {
            const raw_buffer = this.buffer.subarray(0, i)
            const packet = cobsDecode(raw_buffer)
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
            // TODO: add protocolVersion to spec
            // if (message.protocolVersion !== PROTOBUF_PROTOCOL_VERSION) {
            //     console.warn(
            //         `Invalid protocol version. Expected ${PROTOBUF_PROTOCOL_VERSION}, received ${message.protocolVersion}`,
            //     )
            //     return
            // }
            if (message.payload === 'ack') {
                const nonce = message.ack?.nonce ?? undefined
                if (nonce === undefined) {
                    console.warn('Received ack without nonce')
                } else {
                    this.handleAck(nonce)
                }
            }
            this.onMessage(message)
        }
    }

    private enqueueMessage(message: PB.ToSplitflap) {
        if (!this.portAvailable) {
            return
        }
        // TODO: add protocolVersion to spec
        // message.protocolVersion = PROTOBUF_PROTOCOL_VERSION
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
            console.log(`Ignoring unexpected ack for nonce ${nonce}`)
        }
    }

    private serviceQueue(): void {
        if (!this.portAvailable) {
            return
        }
        if (this.retryTimeout !== null) {
            // Retry is pending; let the pending timeout handle the next step
            return
        }
        if (this.outgoingQueue.length === 0) {
            return
        }
        const {encodedToSplitflapPayload: payload} = this.outgoingQueue[0]

        const crc = CRC32.buf(payload)
        const crcArray = [crc & 0xff, (crc >>> 8) & 0xff, (crc >>> 16) & 0xff, (crc >>> 24) & 0xff]

        const packet = new Uint8Array(payload.length + 4)
        packet.set(payload, 0)
        packet.set(crcArray, payload.length)

        const cobsEncodedPacket = cobsEncode(packet)

        const encodedDelimitedPacket = new Uint8Array(cobsEncodedPacket.length + 1)
        encodedDelimitedPacket.set(cobsEncodedPacket, 0)
        encodedDelimitedPacket.set([0], cobsEncodedPacket.length)

        this.retryTimeout = setTimeout(() => {
            this.retryTimeout = null
            console.log(`Retrying ToSplitflap...`)
            this.serviceQueue()
        }, SplitflapCore.RETRY_MILLIS)

        console.debug(
            `Sent ${payload.length} byte payload with CRC ${(crc >>> 0).toString(16)} (${
                cobsEncodedPacket.length
            } bytes encoded)`,
            encodedDelimitedPacket,
        )
        this.sendBytes(encodedDelimitedPacket)
    }
}
