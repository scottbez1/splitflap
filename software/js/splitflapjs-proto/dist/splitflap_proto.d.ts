import * as $protobuf from "protobufjs";
/** Namespace PB. */
export namespace PB {

    /** Properties of a SplitflapState. */
    interface ISplitflapState {

        /** SplitflapState modules */
        modules?: (PB.SplitflapState.IModuleState[]|null);
    }

    /** Represents a SplitflapState. */
    class SplitflapState implements ISplitflapState {

        /**
         * Constructs a new SplitflapState.
         * @param [properties] Properties to set
         */
        constructor(properties?: PB.ISplitflapState);

        /** SplitflapState modules. */
        public modules: PB.SplitflapState.IModuleState[];

        /**
         * Creates a new SplitflapState instance using the specified properties.
         * @param [properties] Properties to set
         * @returns SplitflapState instance
         */
        public static create(properties?: PB.ISplitflapState): PB.SplitflapState;

        /**
         * Encodes the specified SplitflapState message. Does not implicitly {@link PB.SplitflapState.verify|verify} messages.
         * @param message SplitflapState message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encode(message: PB.ISplitflapState, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Encodes the specified SplitflapState message, length delimited. Does not implicitly {@link PB.SplitflapState.verify|verify} messages.
         * @param message SplitflapState message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encodeDelimited(message: PB.ISplitflapState, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Decodes a SplitflapState message from the specified reader or buffer.
         * @param reader Reader or buffer to decode from
         * @param [length] Message length if known beforehand
         * @returns SplitflapState
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.SplitflapState;

        /**
         * Decodes a SplitflapState message from the specified reader or buffer, length delimited.
         * @param reader Reader or buffer to decode from
         * @returns SplitflapState
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.SplitflapState;

        /**
         * Verifies a SplitflapState message.
         * @param message Plain object to verify
         * @returns `null` if valid, otherwise the reason why it is not
         */
        public static verify(message: { [k: string]: any }): (string|null);

        /**
         * Creates a SplitflapState message from a plain object. Also converts values to their respective internal types.
         * @param object Plain object
         * @returns SplitflapState
         */
        public static fromObject(object: { [k: string]: any }): PB.SplitflapState;

        /**
         * Creates a plain object from a SplitflapState message. Also converts values to other types if specified.
         * @param message SplitflapState
         * @param [options] Conversion options
         * @returns Plain object
         */
        public static toObject(message: PB.SplitflapState, options?: $protobuf.IConversionOptions): { [k: string]: any };

        /**
         * Converts this SplitflapState to JSON.
         * @returns JSON object
         */
        public toJSON(): { [k: string]: any };
    }

    namespace SplitflapState {

        /** Properties of a ModuleState. */
        interface IModuleState {

            /** ModuleState state */
            state?: (PB.SplitflapState.ModuleState.State|null);

            /** ModuleState flapIndex */
            flapIndex?: (number|null);

            /** ModuleState moving */
            moving?: (boolean|null);

            /** ModuleState homeState */
            homeState?: (boolean|null);

            /** ModuleState countUnexpectedHome */
            countUnexpectedHome?: (number|null);

            /** ModuleState countMissedHome */
            countMissedHome?: (number|null);
        }

        /** Represents a ModuleState. */
        class ModuleState implements IModuleState {

            /**
             * Constructs a new ModuleState.
             * @param [properties] Properties to set
             */
            constructor(properties?: PB.SplitflapState.IModuleState);

            /** ModuleState state. */
            public state: PB.SplitflapState.ModuleState.State;

            /** ModuleState flapIndex. */
            public flapIndex: number;

            /** ModuleState moving. */
            public moving: boolean;

            /** ModuleState homeState. */
            public homeState: boolean;

            /** ModuleState countUnexpectedHome. */
            public countUnexpectedHome: number;

            /** ModuleState countMissedHome. */
            public countMissedHome: number;

            /**
             * Creates a new ModuleState instance using the specified properties.
             * @param [properties] Properties to set
             * @returns ModuleState instance
             */
            public static create(properties?: PB.SplitflapState.IModuleState): PB.SplitflapState.ModuleState;

            /**
             * Encodes the specified ModuleState message. Does not implicitly {@link PB.SplitflapState.ModuleState.verify|verify} messages.
             * @param message ModuleState message or plain object to encode
             * @param [writer] Writer to encode to
             * @returns Writer
             */
            public static encode(message: PB.SplitflapState.IModuleState, writer?: $protobuf.Writer): $protobuf.Writer;

            /**
             * Encodes the specified ModuleState message, length delimited. Does not implicitly {@link PB.SplitflapState.ModuleState.verify|verify} messages.
             * @param message ModuleState message or plain object to encode
             * @param [writer] Writer to encode to
             * @returns Writer
             */
            public static encodeDelimited(message: PB.SplitflapState.IModuleState, writer?: $protobuf.Writer): $protobuf.Writer;

            /**
             * Decodes a ModuleState message from the specified reader or buffer.
             * @param reader Reader or buffer to decode from
             * @param [length] Message length if known beforehand
             * @returns ModuleState
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.SplitflapState.ModuleState;

            /**
             * Decodes a ModuleState message from the specified reader or buffer, length delimited.
             * @param reader Reader or buffer to decode from
             * @returns ModuleState
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.SplitflapState.ModuleState;

            /**
             * Verifies a ModuleState message.
             * @param message Plain object to verify
             * @returns `null` if valid, otherwise the reason why it is not
             */
            public static verify(message: { [k: string]: any }): (string|null);

            /**
             * Creates a ModuleState message from a plain object. Also converts values to their respective internal types.
             * @param object Plain object
             * @returns ModuleState
             */
            public static fromObject(object: { [k: string]: any }): PB.SplitflapState.ModuleState;

            /**
             * Creates a plain object from a ModuleState message. Also converts values to other types if specified.
             * @param message ModuleState
             * @param [options] Conversion options
             * @returns Plain object
             */
            public static toObject(message: PB.SplitflapState.ModuleState, options?: $protobuf.IConversionOptions): { [k: string]: any };

            /**
             * Converts this ModuleState to JSON.
             * @returns JSON object
             */
            public toJSON(): { [k: string]: any };
        }

        namespace ModuleState {

            /** State enum. */
            enum State {
                NORMAL = 0,
                LOOK_FOR_HOME = 1,
                SENSOR_ERROR = 2,
                PANIC = 3,
                STATE_DISABLED = 4
            }
        }
    }

    /** Properties of a Log. */
    interface ILog {

        /** Log msg */
        msg?: (string|null);
    }

    /** Represents a Log. */
    class Log implements ILog {

        /**
         * Constructs a new Log.
         * @param [properties] Properties to set
         */
        constructor(properties?: PB.ILog);

        /** Log msg. */
        public msg: string;

        /**
         * Creates a new Log instance using the specified properties.
         * @param [properties] Properties to set
         * @returns Log instance
         */
        public static create(properties?: PB.ILog): PB.Log;

        /**
         * Encodes the specified Log message. Does not implicitly {@link PB.Log.verify|verify} messages.
         * @param message Log message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encode(message: PB.ILog, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Encodes the specified Log message, length delimited. Does not implicitly {@link PB.Log.verify|verify} messages.
         * @param message Log message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encodeDelimited(message: PB.ILog, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Decodes a Log message from the specified reader or buffer.
         * @param reader Reader or buffer to decode from
         * @param [length] Message length if known beforehand
         * @returns Log
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.Log;

        /**
         * Decodes a Log message from the specified reader or buffer, length delimited.
         * @param reader Reader or buffer to decode from
         * @returns Log
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.Log;

        /**
         * Verifies a Log message.
         * @param message Plain object to verify
         * @returns `null` if valid, otherwise the reason why it is not
         */
        public static verify(message: { [k: string]: any }): (string|null);

        /**
         * Creates a Log message from a plain object. Also converts values to their respective internal types.
         * @param object Plain object
         * @returns Log
         */
        public static fromObject(object: { [k: string]: any }): PB.Log;

        /**
         * Creates a plain object from a Log message. Also converts values to other types if specified.
         * @param message Log
         * @param [options] Conversion options
         * @returns Plain object
         */
        public static toObject(message: PB.Log, options?: $protobuf.IConversionOptions): { [k: string]: any };

        /**
         * Converts this Log to JSON.
         * @returns JSON object
         */
        public toJSON(): { [k: string]: any };
    }

    /** Properties of an Ack. */
    interface IAck {

        /** Ack nonce */
        nonce?: (number|null);
    }

    /** Represents an Ack. */
    class Ack implements IAck {

        /**
         * Constructs a new Ack.
         * @param [properties] Properties to set
         */
        constructor(properties?: PB.IAck);

        /** Ack nonce. */
        public nonce: number;

        /**
         * Creates a new Ack instance using the specified properties.
         * @param [properties] Properties to set
         * @returns Ack instance
         */
        public static create(properties?: PB.IAck): PB.Ack;

        /**
         * Encodes the specified Ack message. Does not implicitly {@link PB.Ack.verify|verify} messages.
         * @param message Ack message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encode(message: PB.IAck, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Encodes the specified Ack message, length delimited. Does not implicitly {@link PB.Ack.verify|verify} messages.
         * @param message Ack message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encodeDelimited(message: PB.IAck, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Decodes an Ack message from the specified reader or buffer.
         * @param reader Reader or buffer to decode from
         * @param [length] Message length if known beforehand
         * @returns Ack
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.Ack;

        /**
         * Decodes an Ack message from the specified reader or buffer, length delimited.
         * @param reader Reader or buffer to decode from
         * @returns Ack
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.Ack;

        /**
         * Verifies an Ack message.
         * @param message Plain object to verify
         * @returns `null` if valid, otherwise the reason why it is not
         */
        public static verify(message: { [k: string]: any }): (string|null);

        /**
         * Creates an Ack message from a plain object. Also converts values to their respective internal types.
         * @param object Plain object
         * @returns Ack
         */
        public static fromObject(object: { [k: string]: any }): PB.Ack;

        /**
         * Creates a plain object from an Ack message. Also converts values to other types if specified.
         * @param message Ack
         * @param [options] Conversion options
         * @returns Plain object
         */
        public static toObject(message: PB.Ack, options?: $protobuf.IConversionOptions): { [k: string]: any };

        /**
         * Converts this Ack to JSON.
         * @returns JSON object
         */
        public toJSON(): { [k: string]: any };
    }

    /** Properties of a SupervisorState. */
    interface ISupervisorState {

        /** SupervisorState uptimeMillis */
        uptimeMillis?: (number|null);

        /** SupervisorState state */
        state?: (PB.SupervisorState.State|null);

        /** SupervisorState powerChannels */
        powerChannels?: (PB.SupervisorState.IPowerChannelState[]|null);

        /** SupervisorState faultInfo */
        faultInfo?: (PB.SupervisorState.IFaultInfo|null);
    }

    /** Represents a SupervisorState. */
    class SupervisorState implements ISupervisorState {

        /**
         * Constructs a new SupervisorState.
         * @param [properties] Properties to set
         */
        constructor(properties?: PB.ISupervisorState);

        /** SupervisorState uptimeMillis. */
        public uptimeMillis: number;

        /** SupervisorState state. */
        public state: PB.SupervisorState.State;

        /** SupervisorState powerChannels. */
        public powerChannels: PB.SupervisorState.IPowerChannelState[];

        /** SupervisorState faultInfo. */
        public faultInfo?: (PB.SupervisorState.IFaultInfo|null);

        /**
         * Creates a new SupervisorState instance using the specified properties.
         * @param [properties] Properties to set
         * @returns SupervisorState instance
         */
        public static create(properties?: PB.ISupervisorState): PB.SupervisorState;

        /**
         * Encodes the specified SupervisorState message. Does not implicitly {@link PB.SupervisorState.verify|verify} messages.
         * @param message SupervisorState message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encode(message: PB.ISupervisorState, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Encodes the specified SupervisorState message, length delimited. Does not implicitly {@link PB.SupervisorState.verify|verify} messages.
         * @param message SupervisorState message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encodeDelimited(message: PB.ISupervisorState, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Decodes a SupervisorState message from the specified reader or buffer.
         * @param reader Reader or buffer to decode from
         * @param [length] Message length if known beforehand
         * @returns SupervisorState
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.SupervisorState;

        /**
         * Decodes a SupervisorState message from the specified reader or buffer, length delimited.
         * @param reader Reader or buffer to decode from
         * @returns SupervisorState
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.SupervisorState;

        /**
         * Verifies a SupervisorState message.
         * @param message Plain object to verify
         * @returns `null` if valid, otherwise the reason why it is not
         */
        public static verify(message: { [k: string]: any }): (string|null);

        /**
         * Creates a SupervisorState message from a plain object. Also converts values to their respective internal types.
         * @param object Plain object
         * @returns SupervisorState
         */
        public static fromObject(object: { [k: string]: any }): PB.SupervisorState;

        /**
         * Creates a plain object from a SupervisorState message. Also converts values to other types if specified.
         * @param message SupervisorState
         * @param [options] Conversion options
         * @returns Plain object
         */
        public static toObject(message: PB.SupervisorState, options?: $protobuf.IConversionOptions): { [k: string]: any };

        /**
         * Converts this SupervisorState to JSON.
         * @returns JSON object
         */
        public toJSON(): { [k: string]: any };
    }

    namespace SupervisorState {

        /** State enum. */
        enum State {
            UNKNOWN = 0,
            STARTING_VERIFY_PSU_OFF = 1,
            STARTING_VERIFY_VOLTAGES = 2,
            STARTING_ENABLE_CHANNELS = 3,
            NORMAL = 4,
            FAULT = 5
        }

        /** Properties of a PowerChannelState. */
        interface IPowerChannelState {

            /** PowerChannelState voltageVolts */
            voltageVolts?: (number|null);

            /** PowerChannelState currentAmps */
            currentAmps?: (number|null);

            /** PowerChannelState on */
            on?: (boolean|null);
        }

        /** Represents a PowerChannelState. */
        class PowerChannelState implements IPowerChannelState {

            /**
             * Constructs a new PowerChannelState.
             * @param [properties] Properties to set
             */
            constructor(properties?: PB.SupervisorState.IPowerChannelState);

            /** PowerChannelState voltageVolts. */
            public voltageVolts: number;

            /** PowerChannelState currentAmps. */
            public currentAmps: number;

            /** PowerChannelState on. */
            public on: boolean;

            /**
             * Creates a new PowerChannelState instance using the specified properties.
             * @param [properties] Properties to set
             * @returns PowerChannelState instance
             */
            public static create(properties?: PB.SupervisorState.IPowerChannelState): PB.SupervisorState.PowerChannelState;

            /**
             * Encodes the specified PowerChannelState message. Does not implicitly {@link PB.SupervisorState.PowerChannelState.verify|verify} messages.
             * @param message PowerChannelState message or plain object to encode
             * @param [writer] Writer to encode to
             * @returns Writer
             */
            public static encode(message: PB.SupervisorState.IPowerChannelState, writer?: $protobuf.Writer): $protobuf.Writer;

            /**
             * Encodes the specified PowerChannelState message, length delimited. Does not implicitly {@link PB.SupervisorState.PowerChannelState.verify|verify} messages.
             * @param message PowerChannelState message or plain object to encode
             * @param [writer] Writer to encode to
             * @returns Writer
             */
            public static encodeDelimited(message: PB.SupervisorState.IPowerChannelState, writer?: $protobuf.Writer): $protobuf.Writer;

            /**
             * Decodes a PowerChannelState message from the specified reader or buffer.
             * @param reader Reader or buffer to decode from
             * @param [length] Message length if known beforehand
             * @returns PowerChannelState
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.SupervisorState.PowerChannelState;

            /**
             * Decodes a PowerChannelState message from the specified reader or buffer, length delimited.
             * @param reader Reader or buffer to decode from
             * @returns PowerChannelState
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.SupervisorState.PowerChannelState;

            /**
             * Verifies a PowerChannelState message.
             * @param message Plain object to verify
             * @returns `null` if valid, otherwise the reason why it is not
             */
            public static verify(message: { [k: string]: any }): (string|null);

            /**
             * Creates a PowerChannelState message from a plain object. Also converts values to their respective internal types.
             * @param object Plain object
             * @returns PowerChannelState
             */
            public static fromObject(object: { [k: string]: any }): PB.SupervisorState.PowerChannelState;

            /**
             * Creates a plain object from a PowerChannelState message. Also converts values to other types if specified.
             * @param message PowerChannelState
             * @param [options] Conversion options
             * @returns Plain object
             */
            public static toObject(message: PB.SupervisorState.PowerChannelState, options?: $protobuf.IConversionOptions): { [k: string]: any };

            /**
             * Converts this PowerChannelState to JSON.
             * @returns JSON object
             */
            public toJSON(): { [k: string]: any };
        }

        /** Properties of a FaultInfo. */
        interface IFaultInfo {

            /** FaultInfo type */
            type?: (PB.SupervisorState.FaultInfo.FaultType|null);

            /** FaultInfo msg */
            msg?: (string|null);

            /** FaultInfo tsMillis */
            tsMillis?: (number|null);
        }

        /** Represents a FaultInfo. */
        class FaultInfo implements IFaultInfo {

            /**
             * Constructs a new FaultInfo.
             * @param [properties] Properties to set
             */
            constructor(properties?: PB.SupervisorState.IFaultInfo);

            /** FaultInfo type. */
            public type: PB.SupervisorState.FaultInfo.FaultType;

            /** FaultInfo msg. */
            public msg: string;

            /** FaultInfo tsMillis. */
            public tsMillis: number;

            /**
             * Creates a new FaultInfo instance using the specified properties.
             * @param [properties] Properties to set
             * @returns FaultInfo instance
             */
            public static create(properties?: PB.SupervisorState.IFaultInfo): PB.SupervisorState.FaultInfo;

            /**
             * Encodes the specified FaultInfo message. Does not implicitly {@link PB.SupervisorState.FaultInfo.verify|verify} messages.
             * @param message FaultInfo message or plain object to encode
             * @param [writer] Writer to encode to
             * @returns Writer
             */
            public static encode(message: PB.SupervisorState.IFaultInfo, writer?: $protobuf.Writer): $protobuf.Writer;

            /**
             * Encodes the specified FaultInfo message, length delimited. Does not implicitly {@link PB.SupervisorState.FaultInfo.verify|verify} messages.
             * @param message FaultInfo message or plain object to encode
             * @param [writer] Writer to encode to
             * @returns Writer
             */
            public static encodeDelimited(message: PB.SupervisorState.IFaultInfo, writer?: $protobuf.Writer): $protobuf.Writer;

            /**
             * Decodes a FaultInfo message from the specified reader or buffer.
             * @param reader Reader or buffer to decode from
             * @param [length] Message length if known beforehand
             * @returns FaultInfo
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.SupervisorState.FaultInfo;

            /**
             * Decodes a FaultInfo message from the specified reader or buffer, length delimited.
             * @param reader Reader or buffer to decode from
             * @returns FaultInfo
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.SupervisorState.FaultInfo;

            /**
             * Verifies a FaultInfo message.
             * @param message Plain object to verify
             * @returns `null` if valid, otherwise the reason why it is not
             */
            public static verify(message: { [k: string]: any }): (string|null);

            /**
             * Creates a FaultInfo message from a plain object. Also converts values to their respective internal types.
             * @param object Plain object
             * @returns FaultInfo
             */
            public static fromObject(object: { [k: string]: any }): PB.SupervisorState.FaultInfo;

            /**
             * Creates a plain object from a FaultInfo message. Also converts values to other types if specified.
             * @param message FaultInfo
             * @param [options] Conversion options
             * @returns Plain object
             */
            public static toObject(message: PB.SupervisorState.FaultInfo, options?: $protobuf.IConversionOptions): { [k: string]: any };

            /**
             * Converts this FaultInfo to JSON.
             * @returns JSON object
             */
            public toJSON(): { [k: string]: any };
        }

        namespace FaultInfo {

            /** FaultType enum. */
            enum FaultType {
                UNKNOWN = 0,
                NONE = 1,
                INRUSH_CURRENT_NOT_SETTLED = 2,
                SPLITFLAP_SHUTDOWN = 3,
                OUT_OF_RANGE = 4,
                OVER_CURRENT = 5,
                UNEXPECTED_POWER = 6
            }
        }
    }

    /** Properties of a FromSplitflap. */
    interface IFromSplitflap {

        /** FromSplitflap splitflapState */
        splitflapState?: (PB.ISplitflapState|null);

        /** FromSplitflap log */
        log?: (PB.ILog|null);

        /** FromSplitflap ack */
        ack?: (PB.IAck|null);

        /** FromSplitflap supervisorState */
        supervisorState?: (PB.ISupervisorState|null);
    }

    /** Represents a FromSplitflap. */
    class FromSplitflap implements IFromSplitflap {

        /**
         * Constructs a new FromSplitflap.
         * @param [properties] Properties to set
         */
        constructor(properties?: PB.IFromSplitflap);

        /** FromSplitflap splitflapState. */
        public splitflapState?: (PB.ISplitflapState|null);

        /** FromSplitflap log. */
        public log?: (PB.ILog|null);

        /** FromSplitflap ack. */
        public ack?: (PB.IAck|null);

        /** FromSplitflap supervisorState. */
        public supervisorState?: (PB.ISupervisorState|null);

        /** FromSplitflap payload. */
        public payload?: ("splitflapState"|"log"|"ack"|"supervisorState");

        /**
         * Creates a new FromSplitflap instance using the specified properties.
         * @param [properties] Properties to set
         * @returns FromSplitflap instance
         */
        public static create(properties?: PB.IFromSplitflap): PB.FromSplitflap;

        /**
         * Encodes the specified FromSplitflap message. Does not implicitly {@link PB.FromSplitflap.verify|verify} messages.
         * @param message FromSplitflap message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encode(message: PB.IFromSplitflap, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Encodes the specified FromSplitflap message, length delimited. Does not implicitly {@link PB.FromSplitflap.verify|verify} messages.
         * @param message FromSplitflap message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encodeDelimited(message: PB.IFromSplitflap, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Decodes a FromSplitflap message from the specified reader or buffer.
         * @param reader Reader or buffer to decode from
         * @param [length] Message length if known beforehand
         * @returns FromSplitflap
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.FromSplitflap;

        /**
         * Decodes a FromSplitflap message from the specified reader or buffer, length delimited.
         * @param reader Reader or buffer to decode from
         * @returns FromSplitflap
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.FromSplitflap;

        /**
         * Verifies a FromSplitflap message.
         * @param message Plain object to verify
         * @returns `null` if valid, otherwise the reason why it is not
         */
        public static verify(message: { [k: string]: any }): (string|null);

        /**
         * Creates a FromSplitflap message from a plain object. Also converts values to their respective internal types.
         * @param object Plain object
         * @returns FromSplitflap
         */
        public static fromObject(object: { [k: string]: any }): PB.FromSplitflap;

        /**
         * Creates a plain object from a FromSplitflap message. Also converts values to other types if specified.
         * @param message FromSplitflap
         * @param [options] Conversion options
         * @returns Plain object
         */
        public static toObject(message: PB.FromSplitflap, options?: $protobuf.IConversionOptions): { [k: string]: any };

        /**
         * Converts this FromSplitflap to JSON.
         * @returns JSON object
         */
        public toJSON(): { [k: string]: any };
    }

    /** Properties of a SplitflapCommand. */
    interface ISplitflapCommand {

        /** SplitflapCommand modules */
        modules?: (PB.SplitflapCommand.IModuleCommand[]|null);
    }

    /** Represents a SplitflapCommand. */
    class SplitflapCommand implements ISplitflapCommand {

        /**
         * Constructs a new SplitflapCommand.
         * @param [properties] Properties to set
         */
        constructor(properties?: PB.ISplitflapCommand);

        /** SplitflapCommand modules. */
        public modules: PB.SplitflapCommand.IModuleCommand[];

        /**
         * Creates a new SplitflapCommand instance using the specified properties.
         * @param [properties] Properties to set
         * @returns SplitflapCommand instance
         */
        public static create(properties?: PB.ISplitflapCommand): PB.SplitflapCommand;

        /**
         * Encodes the specified SplitflapCommand message. Does not implicitly {@link PB.SplitflapCommand.verify|verify} messages.
         * @param message SplitflapCommand message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encode(message: PB.ISplitflapCommand, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Encodes the specified SplitflapCommand message, length delimited. Does not implicitly {@link PB.SplitflapCommand.verify|verify} messages.
         * @param message SplitflapCommand message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encodeDelimited(message: PB.ISplitflapCommand, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Decodes a SplitflapCommand message from the specified reader or buffer.
         * @param reader Reader or buffer to decode from
         * @param [length] Message length if known beforehand
         * @returns SplitflapCommand
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.SplitflapCommand;

        /**
         * Decodes a SplitflapCommand message from the specified reader or buffer, length delimited.
         * @param reader Reader or buffer to decode from
         * @returns SplitflapCommand
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.SplitflapCommand;

        /**
         * Verifies a SplitflapCommand message.
         * @param message Plain object to verify
         * @returns `null` if valid, otherwise the reason why it is not
         */
        public static verify(message: { [k: string]: any }): (string|null);

        /**
         * Creates a SplitflapCommand message from a plain object. Also converts values to their respective internal types.
         * @param object Plain object
         * @returns SplitflapCommand
         */
        public static fromObject(object: { [k: string]: any }): PB.SplitflapCommand;

        /**
         * Creates a plain object from a SplitflapCommand message. Also converts values to other types if specified.
         * @param message SplitflapCommand
         * @param [options] Conversion options
         * @returns Plain object
         */
        public static toObject(message: PB.SplitflapCommand, options?: $protobuf.IConversionOptions): { [k: string]: any };

        /**
         * Converts this SplitflapCommand to JSON.
         * @returns JSON object
         */
        public toJSON(): { [k: string]: any };
    }

    namespace SplitflapCommand {

        /** Properties of a ModuleCommand. */
        interface IModuleCommand {

            /** ModuleCommand action */
            action?: (PB.SplitflapCommand.ModuleCommand.Action|null);

            /** ModuleCommand param */
            param?: (number|null);
        }

        /** Represents a ModuleCommand. */
        class ModuleCommand implements IModuleCommand {

            /**
             * Constructs a new ModuleCommand.
             * @param [properties] Properties to set
             */
            constructor(properties?: PB.SplitflapCommand.IModuleCommand);

            /** ModuleCommand action. */
            public action: PB.SplitflapCommand.ModuleCommand.Action;

            /** ModuleCommand param. */
            public param: number;

            /**
             * Creates a new ModuleCommand instance using the specified properties.
             * @param [properties] Properties to set
             * @returns ModuleCommand instance
             */
            public static create(properties?: PB.SplitflapCommand.IModuleCommand): PB.SplitflapCommand.ModuleCommand;

            /**
             * Encodes the specified ModuleCommand message. Does not implicitly {@link PB.SplitflapCommand.ModuleCommand.verify|verify} messages.
             * @param message ModuleCommand message or plain object to encode
             * @param [writer] Writer to encode to
             * @returns Writer
             */
            public static encode(message: PB.SplitflapCommand.IModuleCommand, writer?: $protobuf.Writer): $protobuf.Writer;

            /**
             * Encodes the specified ModuleCommand message, length delimited. Does not implicitly {@link PB.SplitflapCommand.ModuleCommand.verify|verify} messages.
             * @param message ModuleCommand message or plain object to encode
             * @param [writer] Writer to encode to
             * @returns Writer
             */
            public static encodeDelimited(message: PB.SplitflapCommand.IModuleCommand, writer?: $protobuf.Writer): $protobuf.Writer;

            /**
             * Decodes a ModuleCommand message from the specified reader or buffer.
             * @param reader Reader or buffer to decode from
             * @param [length] Message length if known beforehand
             * @returns ModuleCommand
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.SplitflapCommand.ModuleCommand;

            /**
             * Decodes a ModuleCommand message from the specified reader or buffer, length delimited.
             * @param reader Reader or buffer to decode from
             * @returns ModuleCommand
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.SplitflapCommand.ModuleCommand;

            /**
             * Verifies a ModuleCommand message.
             * @param message Plain object to verify
             * @returns `null` if valid, otherwise the reason why it is not
             */
            public static verify(message: { [k: string]: any }): (string|null);

            /**
             * Creates a ModuleCommand message from a plain object. Also converts values to their respective internal types.
             * @param object Plain object
             * @returns ModuleCommand
             */
            public static fromObject(object: { [k: string]: any }): PB.SplitflapCommand.ModuleCommand;

            /**
             * Creates a plain object from a ModuleCommand message. Also converts values to other types if specified.
             * @param message ModuleCommand
             * @param [options] Conversion options
             * @returns Plain object
             */
            public static toObject(message: PB.SplitflapCommand.ModuleCommand, options?: $protobuf.IConversionOptions): { [k: string]: any };

            /**
             * Converts this ModuleCommand to JSON.
             * @returns JSON object
             */
            public toJSON(): { [k: string]: any };
        }

        namespace ModuleCommand {

            /** Action enum. */
            enum Action {
                NO_OP = 0,
                GO_TO_FLAP = 1,
                RESET_AND_HOME = 2
            }
        }
    }

    /** Properties of a SplitflapConfig. */
    interface ISplitflapConfig {

        /** SplitflapConfig modules */
        modules?: (PB.SplitflapConfig.IModuleConfig[]|null);
    }

    /** Represents a SplitflapConfig. */
    class SplitflapConfig implements ISplitflapConfig {

        /**
         * Constructs a new SplitflapConfig.
         * @param [properties] Properties to set
         */
        constructor(properties?: PB.ISplitflapConfig);

        /** SplitflapConfig modules. */
        public modules: PB.SplitflapConfig.IModuleConfig[];

        /**
         * Creates a new SplitflapConfig instance using the specified properties.
         * @param [properties] Properties to set
         * @returns SplitflapConfig instance
         */
        public static create(properties?: PB.ISplitflapConfig): PB.SplitflapConfig;

        /**
         * Encodes the specified SplitflapConfig message. Does not implicitly {@link PB.SplitflapConfig.verify|verify} messages.
         * @param message SplitflapConfig message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encode(message: PB.ISplitflapConfig, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Encodes the specified SplitflapConfig message, length delimited. Does not implicitly {@link PB.SplitflapConfig.verify|verify} messages.
         * @param message SplitflapConfig message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encodeDelimited(message: PB.ISplitflapConfig, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Decodes a SplitflapConfig message from the specified reader or buffer.
         * @param reader Reader or buffer to decode from
         * @param [length] Message length if known beforehand
         * @returns SplitflapConfig
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.SplitflapConfig;

        /**
         * Decodes a SplitflapConfig message from the specified reader or buffer, length delimited.
         * @param reader Reader or buffer to decode from
         * @returns SplitflapConfig
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.SplitflapConfig;

        /**
         * Verifies a SplitflapConfig message.
         * @param message Plain object to verify
         * @returns `null` if valid, otherwise the reason why it is not
         */
        public static verify(message: { [k: string]: any }): (string|null);

        /**
         * Creates a SplitflapConfig message from a plain object. Also converts values to their respective internal types.
         * @param object Plain object
         * @returns SplitflapConfig
         */
        public static fromObject(object: { [k: string]: any }): PB.SplitflapConfig;

        /**
         * Creates a plain object from a SplitflapConfig message. Also converts values to other types if specified.
         * @param message SplitflapConfig
         * @param [options] Conversion options
         * @returns Plain object
         */
        public static toObject(message: PB.SplitflapConfig, options?: $protobuf.IConversionOptions): { [k: string]: any };

        /**
         * Converts this SplitflapConfig to JSON.
         * @returns JSON object
         */
        public toJSON(): { [k: string]: any };
    }

    namespace SplitflapConfig {

        /** Properties of a ModuleConfig. */
        interface IModuleConfig {

            /** ModuleConfig targetFlapIndex */
            targetFlapIndex?: (number|null);

            /**
             * Value that triggers a movement upon change. If unused, only changes to target_flap_index
             * will trigger a movement. This can be used to trigger a full revolution back to the *same*
             * flap index.
             *
             * NOTE: Must be < 256
             */
            movementNonce?: (number|null);

            /**
             * Value that triggers a reset (clear error counters, re-home) upon change. If unused,
             * module will only re-home upon recoverable errors, and error counters will continue
             * to increase until overflow.
             *
             * NOTE: Must be < 256
             */
            resetNonce?: (number|null);
        }

        /** Represents a ModuleConfig. */
        class ModuleConfig implements IModuleConfig {

            /**
             * Constructs a new ModuleConfig.
             * @param [properties] Properties to set
             */
            constructor(properties?: PB.SplitflapConfig.IModuleConfig);

            /** ModuleConfig targetFlapIndex. */
            public targetFlapIndex: number;

            /**
             * Value that triggers a movement upon change. If unused, only changes to target_flap_index
             * will trigger a movement. This can be used to trigger a full revolution back to the *same*
             * flap index.
             *
             * NOTE: Must be < 256
             */
            public movementNonce: number;

            /**
             * Value that triggers a reset (clear error counters, re-home) upon change. If unused,
             * module will only re-home upon recoverable errors, and error counters will continue
             * to increase until overflow.
             *
             * NOTE: Must be < 256
             */
            public resetNonce: number;

            /**
             * Creates a new ModuleConfig instance using the specified properties.
             * @param [properties] Properties to set
             * @returns ModuleConfig instance
             */
            public static create(properties?: PB.SplitflapConfig.IModuleConfig): PB.SplitflapConfig.ModuleConfig;

            /**
             * Encodes the specified ModuleConfig message. Does not implicitly {@link PB.SplitflapConfig.ModuleConfig.verify|verify} messages.
             * @param message ModuleConfig message or plain object to encode
             * @param [writer] Writer to encode to
             * @returns Writer
             */
            public static encode(message: PB.SplitflapConfig.IModuleConfig, writer?: $protobuf.Writer): $protobuf.Writer;

            /**
             * Encodes the specified ModuleConfig message, length delimited. Does not implicitly {@link PB.SplitflapConfig.ModuleConfig.verify|verify} messages.
             * @param message ModuleConfig message or plain object to encode
             * @param [writer] Writer to encode to
             * @returns Writer
             */
            public static encodeDelimited(message: PB.SplitflapConfig.IModuleConfig, writer?: $protobuf.Writer): $protobuf.Writer;

            /**
             * Decodes a ModuleConfig message from the specified reader or buffer.
             * @param reader Reader or buffer to decode from
             * @param [length] Message length if known beforehand
             * @returns ModuleConfig
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.SplitflapConfig.ModuleConfig;

            /**
             * Decodes a ModuleConfig message from the specified reader or buffer, length delimited.
             * @param reader Reader or buffer to decode from
             * @returns ModuleConfig
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.SplitflapConfig.ModuleConfig;

            /**
             * Verifies a ModuleConfig message.
             * @param message Plain object to verify
             * @returns `null` if valid, otherwise the reason why it is not
             */
            public static verify(message: { [k: string]: any }): (string|null);

            /**
             * Creates a ModuleConfig message from a plain object. Also converts values to their respective internal types.
             * @param object Plain object
             * @returns ModuleConfig
             */
            public static fromObject(object: { [k: string]: any }): PB.SplitflapConfig.ModuleConfig;

            /**
             * Creates a plain object from a ModuleConfig message. Also converts values to other types if specified.
             * @param message ModuleConfig
             * @param [options] Conversion options
             * @returns Plain object
             */
            public static toObject(message: PB.SplitflapConfig.ModuleConfig, options?: $protobuf.IConversionOptions): { [k: string]: any };

            /**
             * Converts this ModuleConfig to JSON.
             * @returns JSON object
             */
            public toJSON(): { [k: string]: any };
        }
    }

    /** Properties of a ToSplitflap. */
    interface IToSplitflap {

        /** ToSplitflap nonce */
        nonce?: (number|null);

        /** ToSplitflap splitflapCommand */
        splitflapCommand?: (PB.ISplitflapCommand|null);

        /** ToSplitflap splitflapConfig */
        splitflapConfig?: (PB.ISplitflapConfig|null);
    }

    /** Represents a ToSplitflap. */
    class ToSplitflap implements IToSplitflap {

        /**
         * Constructs a new ToSplitflap.
         * @param [properties] Properties to set
         */
        constructor(properties?: PB.IToSplitflap);

        /** ToSplitflap nonce. */
        public nonce: number;

        /** ToSplitflap splitflapCommand. */
        public splitflapCommand?: (PB.ISplitflapCommand|null);

        /** ToSplitflap splitflapConfig. */
        public splitflapConfig?: (PB.ISplitflapConfig|null);

        /** ToSplitflap payload. */
        public payload?: ("splitflapCommand"|"splitflapConfig");

        /**
         * Creates a new ToSplitflap instance using the specified properties.
         * @param [properties] Properties to set
         * @returns ToSplitflap instance
         */
        public static create(properties?: PB.IToSplitflap): PB.ToSplitflap;

        /**
         * Encodes the specified ToSplitflap message. Does not implicitly {@link PB.ToSplitflap.verify|verify} messages.
         * @param message ToSplitflap message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encode(message: PB.IToSplitflap, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Encodes the specified ToSplitflap message, length delimited. Does not implicitly {@link PB.ToSplitflap.verify|verify} messages.
         * @param message ToSplitflap message or plain object to encode
         * @param [writer] Writer to encode to
         * @returns Writer
         */
        public static encodeDelimited(message: PB.IToSplitflap, writer?: $protobuf.Writer): $protobuf.Writer;

        /**
         * Decodes a ToSplitflap message from the specified reader or buffer.
         * @param reader Reader or buffer to decode from
         * @param [length] Message length if known beforehand
         * @returns ToSplitflap
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decode(reader: ($protobuf.Reader|Uint8Array), length?: number): PB.ToSplitflap;

        /**
         * Decodes a ToSplitflap message from the specified reader or buffer, length delimited.
         * @param reader Reader or buffer to decode from
         * @returns ToSplitflap
         * @throws {Error} If the payload is not a reader or valid buffer
         * @throws {$protobuf.util.ProtocolError} If required fields are missing
         */
        public static decodeDelimited(reader: ($protobuf.Reader|Uint8Array)): PB.ToSplitflap;

        /**
         * Verifies a ToSplitflap message.
         * @param message Plain object to verify
         * @returns `null` if valid, otherwise the reason why it is not
         */
        public static verify(message: { [k: string]: any }): (string|null);

        /**
         * Creates a ToSplitflap message from a plain object. Also converts values to their respective internal types.
         * @param object Plain object
         * @returns ToSplitflap
         */
        public static fromObject(object: { [k: string]: any }): PB.ToSplitflap;

        /**
         * Creates a plain object from a ToSplitflap message. Also converts values to other types if specified.
         * @param message ToSplitflap
         * @param [options] Conversion options
         * @returns Plain object
         */
        public static toObject(message: PB.ToSplitflap, options?: $protobuf.IConversionOptions): { [k: string]: any };

        /**
         * Converts this ToSplitflap to JSON.
         * @returns JSON object
         */
        public toJSON(): { [k: string]: any };
    }
}
