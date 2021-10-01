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

    /** Properties of a FromSplitflap. */
    interface IFromSplitflap {

        /** FromSplitflap splitflapState */
        splitflapState?: (PB.ISplitflapState|null);

        /** FromSplitflap log */
        log?: (PB.ILog|null);
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

        /** FromSplitflap payload. */
        public payload?: ("splitflapState"|"log");

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

    /** Properties of a ToSplitflap. */
    interface IToSplitflap {

        /** ToSplitflap splitflapCommand */
        splitflapCommand?: (PB.ISplitflapCommand|null);
    }

    /** Represents a ToSplitflap. */
    class ToSplitflap implements IToSplitflap {

        /**
         * Constructs a new ToSplitflap.
         * @param [properties] Properties to set
         */
        constructor(properties?: PB.IToSplitflap);

        /** ToSplitflap splitflapCommand. */
        public splitflapCommand?: (PB.ISplitflapCommand|null);

        /** ToSplitflap payload. */
        public payload?: "splitflapCommand";

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
