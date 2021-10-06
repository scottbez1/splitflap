/*eslint-disable block-scoped-var, id-length, no-control-regex, no-magic-numbers, no-prototype-builtins, no-redeclare, no-shadow, no-var, sort-vars*/
(function(global, factory) { /* global define, require, module */

    /* AMD */ if (typeof define === 'function' && define.amd)
        define(["protobufjs/minimal"], factory);

    /* CommonJS */ else if (typeof require === 'function' && typeof module === 'object' && module && module.exports)
        module.exports = factory(require("protobufjs/minimal"));

})(this, function($protobuf) {
    "use strict";

    // Common aliases
    var $Reader = $protobuf.Reader, $Writer = $protobuf.Writer, $util = $protobuf.util;
    
    // Exported root namespace
    var $root = $protobuf.roots["default"] || ($protobuf.roots["default"] = {});
    
    $root.PB = (function() {
    
        /**
         * Namespace PB.
         * @exports PB
         * @namespace
         */
        var PB = {};
    
        PB.SplitflapState = (function() {
    
            /**
             * Properties of a SplitflapState.
             * @memberof PB
             * @interface ISplitflapState
             * @property {Array.<PB.SplitflapState.IModuleState>|null} [modules] SplitflapState modules
             */
    
            /**
             * Constructs a new SplitflapState.
             * @memberof PB
             * @classdesc Represents a SplitflapState.
             * @implements ISplitflapState
             * @constructor
             * @param {PB.ISplitflapState=} [properties] Properties to set
             */
            function SplitflapState(properties) {
                this.modules = [];
                if (properties)
                    for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                        if (properties[keys[i]] != null)
                            this[keys[i]] = properties[keys[i]];
            }
    
            /**
             * SplitflapState modules.
             * @member {Array.<PB.SplitflapState.IModuleState>} modules
             * @memberof PB.SplitflapState
             * @instance
             */
            SplitflapState.prototype.modules = $util.emptyArray;
    
            /**
             * Creates a new SplitflapState instance using the specified properties.
             * @function create
             * @memberof PB.SplitflapState
             * @static
             * @param {PB.ISplitflapState=} [properties] Properties to set
             * @returns {PB.SplitflapState} SplitflapState instance
             */
            SplitflapState.create = function create(properties) {
                return new SplitflapState(properties);
            };
    
            /**
             * Encodes the specified SplitflapState message. Does not implicitly {@link PB.SplitflapState.verify|verify} messages.
             * @function encode
             * @memberof PB.SplitflapState
             * @static
             * @param {PB.ISplitflapState} message SplitflapState message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            SplitflapState.encode = function encode(message, writer) {
                if (!writer)
                    writer = $Writer.create();
                if (message.modules != null && message.modules.length)
                    for (var i = 0; i < message.modules.length; ++i)
                        $root.PB.SplitflapState.ModuleState.encode(message.modules[i], writer.uint32(/* id 1, wireType 2 =*/10).fork()).ldelim();
                return writer;
            };
    
            /**
             * Encodes the specified SplitflapState message, length delimited. Does not implicitly {@link PB.SplitflapState.verify|verify} messages.
             * @function encodeDelimited
             * @memberof PB.SplitflapState
             * @static
             * @param {PB.ISplitflapState} message SplitflapState message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            SplitflapState.encodeDelimited = function encodeDelimited(message, writer) {
                return this.encode(message, writer).ldelim();
            };
    
            /**
             * Decodes a SplitflapState message from the specified reader or buffer.
             * @function decode
             * @memberof PB.SplitflapState
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @param {number} [length] Message length if known beforehand
             * @returns {PB.SplitflapState} SplitflapState
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            SplitflapState.decode = function decode(reader, length) {
                if (!(reader instanceof $Reader))
                    reader = $Reader.create(reader);
                var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.SplitflapState();
                while (reader.pos < end) {
                    var tag = reader.uint32();
                    switch (tag >>> 3) {
                    case 1:
                        if (!(message.modules && message.modules.length))
                            message.modules = [];
                        message.modules.push($root.PB.SplitflapState.ModuleState.decode(reader, reader.uint32()));
                        break;
                    default:
                        reader.skipType(tag & 7);
                        break;
                    }
                }
                return message;
            };
    
            /**
             * Decodes a SplitflapState message from the specified reader or buffer, length delimited.
             * @function decodeDelimited
             * @memberof PB.SplitflapState
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @returns {PB.SplitflapState} SplitflapState
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            SplitflapState.decodeDelimited = function decodeDelimited(reader) {
                if (!(reader instanceof $Reader))
                    reader = new $Reader(reader);
                return this.decode(reader, reader.uint32());
            };
    
            /**
             * Verifies a SplitflapState message.
             * @function verify
             * @memberof PB.SplitflapState
             * @static
             * @param {Object.<string,*>} message Plain object to verify
             * @returns {string|null} `null` if valid, otherwise the reason why it is not
             */
            SplitflapState.verify = function verify(message) {
                if (typeof message !== "object" || message === null)
                    return "object expected";
                if (message.modules != null && message.hasOwnProperty("modules")) {
                    if (!Array.isArray(message.modules))
                        return "modules: array expected";
                    for (var i = 0; i < message.modules.length; ++i) {
                        var error = $root.PB.SplitflapState.ModuleState.verify(message.modules[i]);
                        if (error)
                            return "modules." + error;
                    }
                }
                return null;
            };
    
            /**
             * Creates a SplitflapState message from a plain object. Also converts values to their respective internal types.
             * @function fromObject
             * @memberof PB.SplitflapState
             * @static
             * @param {Object.<string,*>} object Plain object
             * @returns {PB.SplitflapState} SplitflapState
             */
            SplitflapState.fromObject = function fromObject(object) {
                if (object instanceof $root.PB.SplitflapState)
                    return object;
                var message = new $root.PB.SplitflapState();
                if (object.modules) {
                    if (!Array.isArray(object.modules))
                        throw TypeError(".PB.SplitflapState.modules: array expected");
                    message.modules = [];
                    for (var i = 0; i < object.modules.length; ++i) {
                        if (typeof object.modules[i] !== "object")
                            throw TypeError(".PB.SplitflapState.modules: object expected");
                        message.modules[i] = $root.PB.SplitflapState.ModuleState.fromObject(object.modules[i]);
                    }
                }
                return message;
            };
    
            /**
             * Creates a plain object from a SplitflapState message. Also converts values to other types if specified.
             * @function toObject
             * @memberof PB.SplitflapState
             * @static
             * @param {PB.SplitflapState} message SplitflapState
             * @param {$protobuf.IConversionOptions} [options] Conversion options
             * @returns {Object.<string,*>} Plain object
             */
            SplitflapState.toObject = function toObject(message, options) {
                if (!options)
                    options = {};
                var object = {};
                if (options.arrays || options.defaults)
                    object.modules = [];
                if (message.modules && message.modules.length) {
                    object.modules = [];
                    for (var j = 0; j < message.modules.length; ++j)
                        object.modules[j] = $root.PB.SplitflapState.ModuleState.toObject(message.modules[j], options);
                }
                return object;
            };
    
            /**
             * Converts this SplitflapState to JSON.
             * @function toJSON
             * @memberof PB.SplitflapState
             * @instance
             * @returns {Object.<string,*>} JSON object
             */
            SplitflapState.prototype.toJSON = function toJSON() {
                return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
            };
    
            SplitflapState.ModuleState = (function() {
    
                /**
                 * Properties of a ModuleState.
                 * @memberof PB.SplitflapState
                 * @interface IModuleState
                 * @property {PB.SplitflapState.ModuleState.State|null} [state] ModuleState state
                 * @property {number|null} [flapIndex] ModuleState flapIndex
                 * @property {boolean|null} [moving] ModuleState moving
                 * @property {boolean|null} [homeState] ModuleState homeState
                 * @property {number|null} [countUnexpectedHome] ModuleState countUnexpectedHome
                 * @property {number|null} [countMissedHome] ModuleState countMissedHome
                 */
    
                /**
                 * Constructs a new ModuleState.
                 * @memberof PB.SplitflapState
                 * @classdesc Represents a ModuleState.
                 * @implements IModuleState
                 * @constructor
                 * @param {PB.SplitflapState.IModuleState=} [properties] Properties to set
                 */
                function ModuleState(properties) {
                    if (properties)
                        for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                            if (properties[keys[i]] != null)
                                this[keys[i]] = properties[keys[i]];
                }
    
                /**
                 * ModuleState state.
                 * @member {PB.SplitflapState.ModuleState.State} state
                 * @memberof PB.SplitflapState.ModuleState
                 * @instance
                 */
                ModuleState.prototype.state = 0;
    
                /**
                 * ModuleState flapIndex.
                 * @member {number} flapIndex
                 * @memberof PB.SplitflapState.ModuleState
                 * @instance
                 */
                ModuleState.prototype.flapIndex = 0;
    
                /**
                 * ModuleState moving.
                 * @member {boolean} moving
                 * @memberof PB.SplitflapState.ModuleState
                 * @instance
                 */
                ModuleState.prototype.moving = false;
    
                /**
                 * ModuleState homeState.
                 * @member {boolean} homeState
                 * @memberof PB.SplitflapState.ModuleState
                 * @instance
                 */
                ModuleState.prototype.homeState = false;
    
                /**
                 * ModuleState countUnexpectedHome.
                 * @member {number} countUnexpectedHome
                 * @memberof PB.SplitflapState.ModuleState
                 * @instance
                 */
                ModuleState.prototype.countUnexpectedHome = 0;
    
                /**
                 * ModuleState countMissedHome.
                 * @member {number} countMissedHome
                 * @memberof PB.SplitflapState.ModuleState
                 * @instance
                 */
                ModuleState.prototype.countMissedHome = 0;
    
                /**
                 * Creates a new ModuleState instance using the specified properties.
                 * @function create
                 * @memberof PB.SplitflapState.ModuleState
                 * @static
                 * @param {PB.SplitflapState.IModuleState=} [properties] Properties to set
                 * @returns {PB.SplitflapState.ModuleState} ModuleState instance
                 */
                ModuleState.create = function create(properties) {
                    return new ModuleState(properties);
                };
    
                /**
                 * Encodes the specified ModuleState message. Does not implicitly {@link PB.SplitflapState.ModuleState.verify|verify} messages.
                 * @function encode
                 * @memberof PB.SplitflapState.ModuleState
                 * @static
                 * @param {PB.SplitflapState.IModuleState} message ModuleState message or plain object to encode
                 * @param {$protobuf.Writer} [writer] Writer to encode to
                 * @returns {$protobuf.Writer} Writer
                 */
                ModuleState.encode = function encode(message, writer) {
                    if (!writer)
                        writer = $Writer.create();
                    if (message.state != null && Object.hasOwnProperty.call(message, "state"))
                        writer.uint32(/* id 1, wireType 0 =*/8).int32(message.state);
                    if (message.flapIndex != null && Object.hasOwnProperty.call(message, "flapIndex"))
                        writer.uint32(/* id 2, wireType 0 =*/16).uint32(message.flapIndex);
                    if (message.moving != null && Object.hasOwnProperty.call(message, "moving"))
                        writer.uint32(/* id 3, wireType 0 =*/24).bool(message.moving);
                    if (message.homeState != null && Object.hasOwnProperty.call(message, "homeState"))
                        writer.uint32(/* id 4, wireType 0 =*/32).bool(message.homeState);
                    if (message.countUnexpectedHome != null && Object.hasOwnProperty.call(message, "countUnexpectedHome"))
                        writer.uint32(/* id 5, wireType 0 =*/40).uint32(message.countUnexpectedHome);
                    if (message.countMissedHome != null && Object.hasOwnProperty.call(message, "countMissedHome"))
                        writer.uint32(/* id 6, wireType 0 =*/48).uint32(message.countMissedHome);
                    return writer;
                };
    
                /**
                 * Encodes the specified ModuleState message, length delimited. Does not implicitly {@link PB.SplitflapState.ModuleState.verify|verify} messages.
                 * @function encodeDelimited
                 * @memberof PB.SplitflapState.ModuleState
                 * @static
                 * @param {PB.SplitflapState.IModuleState} message ModuleState message or plain object to encode
                 * @param {$protobuf.Writer} [writer] Writer to encode to
                 * @returns {$protobuf.Writer} Writer
                 */
                ModuleState.encodeDelimited = function encodeDelimited(message, writer) {
                    return this.encode(message, writer).ldelim();
                };
    
                /**
                 * Decodes a ModuleState message from the specified reader or buffer.
                 * @function decode
                 * @memberof PB.SplitflapState.ModuleState
                 * @static
                 * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
                 * @param {number} [length] Message length if known beforehand
                 * @returns {PB.SplitflapState.ModuleState} ModuleState
                 * @throws {Error} If the payload is not a reader or valid buffer
                 * @throws {$protobuf.util.ProtocolError} If required fields are missing
                 */
                ModuleState.decode = function decode(reader, length) {
                    if (!(reader instanceof $Reader))
                        reader = $Reader.create(reader);
                    var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.SplitflapState.ModuleState();
                    while (reader.pos < end) {
                        var tag = reader.uint32();
                        switch (tag >>> 3) {
                        case 1:
                            message.state = reader.int32();
                            break;
                        case 2:
                            message.flapIndex = reader.uint32();
                            break;
                        case 3:
                            message.moving = reader.bool();
                            break;
                        case 4:
                            message.homeState = reader.bool();
                            break;
                        case 5:
                            message.countUnexpectedHome = reader.uint32();
                            break;
                        case 6:
                            message.countMissedHome = reader.uint32();
                            break;
                        default:
                            reader.skipType(tag & 7);
                            break;
                        }
                    }
                    return message;
                };
    
                /**
                 * Decodes a ModuleState message from the specified reader or buffer, length delimited.
                 * @function decodeDelimited
                 * @memberof PB.SplitflapState.ModuleState
                 * @static
                 * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
                 * @returns {PB.SplitflapState.ModuleState} ModuleState
                 * @throws {Error} If the payload is not a reader or valid buffer
                 * @throws {$protobuf.util.ProtocolError} If required fields are missing
                 */
                ModuleState.decodeDelimited = function decodeDelimited(reader) {
                    if (!(reader instanceof $Reader))
                        reader = new $Reader(reader);
                    return this.decode(reader, reader.uint32());
                };
    
                /**
                 * Verifies a ModuleState message.
                 * @function verify
                 * @memberof PB.SplitflapState.ModuleState
                 * @static
                 * @param {Object.<string,*>} message Plain object to verify
                 * @returns {string|null} `null` if valid, otherwise the reason why it is not
                 */
                ModuleState.verify = function verify(message) {
                    if (typeof message !== "object" || message === null)
                        return "object expected";
                    if (message.state != null && message.hasOwnProperty("state"))
                        switch (message.state) {
                        default:
                            return "state: enum value expected";
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                            break;
                        }
                    if (message.flapIndex != null && message.hasOwnProperty("flapIndex"))
                        if (!$util.isInteger(message.flapIndex))
                            return "flapIndex: integer expected";
                    if (message.moving != null && message.hasOwnProperty("moving"))
                        if (typeof message.moving !== "boolean")
                            return "moving: boolean expected";
                    if (message.homeState != null && message.hasOwnProperty("homeState"))
                        if (typeof message.homeState !== "boolean")
                            return "homeState: boolean expected";
                    if (message.countUnexpectedHome != null && message.hasOwnProperty("countUnexpectedHome"))
                        if (!$util.isInteger(message.countUnexpectedHome))
                            return "countUnexpectedHome: integer expected";
                    if (message.countMissedHome != null && message.hasOwnProperty("countMissedHome"))
                        if (!$util.isInteger(message.countMissedHome))
                            return "countMissedHome: integer expected";
                    return null;
                };
    
                /**
                 * Creates a ModuleState message from a plain object. Also converts values to their respective internal types.
                 * @function fromObject
                 * @memberof PB.SplitflapState.ModuleState
                 * @static
                 * @param {Object.<string,*>} object Plain object
                 * @returns {PB.SplitflapState.ModuleState} ModuleState
                 */
                ModuleState.fromObject = function fromObject(object) {
                    if (object instanceof $root.PB.SplitflapState.ModuleState)
                        return object;
                    var message = new $root.PB.SplitflapState.ModuleState();
                    switch (object.state) {
                    case "NORMAL":
                    case 0:
                        message.state = 0;
                        break;
                    case "LOOK_FOR_HOME":
                    case 1:
                        message.state = 1;
                        break;
                    case "SENSOR_ERROR":
                    case 2:
                        message.state = 2;
                        break;
                    case "PANIC":
                    case 3:
                        message.state = 3;
                        break;
                    case "STATE_DISABLED":
                    case 4:
                        message.state = 4;
                        break;
                    }
                    if (object.flapIndex != null)
                        message.flapIndex = object.flapIndex >>> 0;
                    if (object.moving != null)
                        message.moving = Boolean(object.moving);
                    if (object.homeState != null)
                        message.homeState = Boolean(object.homeState);
                    if (object.countUnexpectedHome != null)
                        message.countUnexpectedHome = object.countUnexpectedHome >>> 0;
                    if (object.countMissedHome != null)
                        message.countMissedHome = object.countMissedHome >>> 0;
                    return message;
                };
    
                /**
                 * Creates a plain object from a ModuleState message. Also converts values to other types if specified.
                 * @function toObject
                 * @memberof PB.SplitflapState.ModuleState
                 * @static
                 * @param {PB.SplitflapState.ModuleState} message ModuleState
                 * @param {$protobuf.IConversionOptions} [options] Conversion options
                 * @returns {Object.<string,*>} Plain object
                 */
                ModuleState.toObject = function toObject(message, options) {
                    if (!options)
                        options = {};
                    var object = {};
                    if (options.defaults) {
                        object.state = options.enums === String ? "NORMAL" : 0;
                        object.flapIndex = 0;
                        object.moving = false;
                        object.homeState = false;
                        object.countUnexpectedHome = 0;
                        object.countMissedHome = 0;
                    }
                    if (message.state != null && message.hasOwnProperty("state"))
                        object.state = options.enums === String ? $root.PB.SplitflapState.ModuleState.State[message.state] : message.state;
                    if (message.flapIndex != null && message.hasOwnProperty("flapIndex"))
                        object.flapIndex = message.flapIndex;
                    if (message.moving != null && message.hasOwnProperty("moving"))
                        object.moving = message.moving;
                    if (message.homeState != null && message.hasOwnProperty("homeState"))
                        object.homeState = message.homeState;
                    if (message.countUnexpectedHome != null && message.hasOwnProperty("countUnexpectedHome"))
                        object.countUnexpectedHome = message.countUnexpectedHome;
                    if (message.countMissedHome != null && message.hasOwnProperty("countMissedHome"))
                        object.countMissedHome = message.countMissedHome;
                    return object;
                };
    
                /**
                 * Converts this ModuleState to JSON.
                 * @function toJSON
                 * @memberof PB.SplitflapState.ModuleState
                 * @instance
                 * @returns {Object.<string,*>} JSON object
                 */
                ModuleState.prototype.toJSON = function toJSON() {
                    return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
                };
    
                /**
                 * State enum.
                 * @name PB.SplitflapState.ModuleState.State
                 * @enum {number}
                 * @property {number} NORMAL=0 NORMAL value
                 * @property {number} LOOK_FOR_HOME=1 LOOK_FOR_HOME value
                 * @property {number} SENSOR_ERROR=2 SENSOR_ERROR value
                 * @property {number} PANIC=3 PANIC value
                 * @property {number} STATE_DISABLED=4 STATE_DISABLED value
                 */
                ModuleState.State = (function() {
                    var valuesById = {}, values = Object.create(valuesById);
                    values[valuesById[0] = "NORMAL"] = 0;
                    values[valuesById[1] = "LOOK_FOR_HOME"] = 1;
                    values[valuesById[2] = "SENSOR_ERROR"] = 2;
                    values[valuesById[3] = "PANIC"] = 3;
                    values[valuesById[4] = "STATE_DISABLED"] = 4;
                    return values;
                })();
    
                return ModuleState;
            })();
    
            return SplitflapState;
        })();
    
        PB.Log = (function() {
    
            /**
             * Properties of a Log.
             * @memberof PB
             * @interface ILog
             * @property {string|null} [msg] Log msg
             */
    
            /**
             * Constructs a new Log.
             * @memberof PB
             * @classdesc Represents a Log.
             * @implements ILog
             * @constructor
             * @param {PB.ILog=} [properties] Properties to set
             */
            function Log(properties) {
                if (properties)
                    for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                        if (properties[keys[i]] != null)
                            this[keys[i]] = properties[keys[i]];
            }
    
            /**
             * Log msg.
             * @member {string} msg
             * @memberof PB.Log
             * @instance
             */
            Log.prototype.msg = "";
    
            /**
             * Creates a new Log instance using the specified properties.
             * @function create
             * @memberof PB.Log
             * @static
             * @param {PB.ILog=} [properties] Properties to set
             * @returns {PB.Log} Log instance
             */
            Log.create = function create(properties) {
                return new Log(properties);
            };
    
            /**
             * Encodes the specified Log message. Does not implicitly {@link PB.Log.verify|verify} messages.
             * @function encode
             * @memberof PB.Log
             * @static
             * @param {PB.ILog} message Log message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            Log.encode = function encode(message, writer) {
                if (!writer)
                    writer = $Writer.create();
                if (message.msg != null && Object.hasOwnProperty.call(message, "msg"))
                    writer.uint32(/* id 1, wireType 2 =*/10).string(message.msg);
                return writer;
            };
    
            /**
             * Encodes the specified Log message, length delimited. Does not implicitly {@link PB.Log.verify|verify} messages.
             * @function encodeDelimited
             * @memberof PB.Log
             * @static
             * @param {PB.ILog} message Log message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            Log.encodeDelimited = function encodeDelimited(message, writer) {
                return this.encode(message, writer).ldelim();
            };
    
            /**
             * Decodes a Log message from the specified reader or buffer.
             * @function decode
             * @memberof PB.Log
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @param {number} [length] Message length if known beforehand
             * @returns {PB.Log} Log
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            Log.decode = function decode(reader, length) {
                if (!(reader instanceof $Reader))
                    reader = $Reader.create(reader);
                var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.Log();
                while (reader.pos < end) {
                    var tag = reader.uint32();
                    switch (tag >>> 3) {
                    case 1:
                        message.msg = reader.string();
                        break;
                    default:
                        reader.skipType(tag & 7);
                        break;
                    }
                }
                return message;
            };
    
            /**
             * Decodes a Log message from the specified reader or buffer, length delimited.
             * @function decodeDelimited
             * @memberof PB.Log
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @returns {PB.Log} Log
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            Log.decodeDelimited = function decodeDelimited(reader) {
                if (!(reader instanceof $Reader))
                    reader = new $Reader(reader);
                return this.decode(reader, reader.uint32());
            };
    
            /**
             * Verifies a Log message.
             * @function verify
             * @memberof PB.Log
             * @static
             * @param {Object.<string,*>} message Plain object to verify
             * @returns {string|null} `null` if valid, otherwise the reason why it is not
             */
            Log.verify = function verify(message) {
                if (typeof message !== "object" || message === null)
                    return "object expected";
                if (message.msg != null && message.hasOwnProperty("msg"))
                    if (!$util.isString(message.msg))
                        return "msg: string expected";
                return null;
            };
    
            /**
             * Creates a Log message from a plain object. Also converts values to their respective internal types.
             * @function fromObject
             * @memberof PB.Log
             * @static
             * @param {Object.<string,*>} object Plain object
             * @returns {PB.Log} Log
             */
            Log.fromObject = function fromObject(object) {
                if (object instanceof $root.PB.Log)
                    return object;
                var message = new $root.PB.Log();
                if (object.msg != null)
                    message.msg = String(object.msg);
                return message;
            };
    
            /**
             * Creates a plain object from a Log message. Also converts values to other types if specified.
             * @function toObject
             * @memberof PB.Log
             * @static
             * @param {PB.Log} message Log
             * @param {$protobuf.IConversionOptions} [options] Conversion options
             * @returns {Object.<string,*>} Plain object
             */
            Log.toObject = function toObject(message, options) {
                if (!options)
                    options = {};
                var object = {};
                if (options.defaults)
                    object.msg = "";
                if (message.msg != null && message.hasOwnProperty("msg"))
                    object.msg = message.msg;
                return object;
            };
    
            /**
             * Converts this Log to JSON.
             * @function toJSON
             * @memberof PB.Log
             * @instance
             * @returns {Object.<string,*>} JSON object
             */
            Log.prototype.toJSON = function toJSON() {
                return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
            };
    
            return Log;
        })();
    
        PB.Ack = (function() {
    
            /**
             * Properties of an Ack.
             * @memberof PB
             * @interface IAck
             * @property {number|null} [nonce] Ack nonce
             */
    
            /**
             * Constructs a new Ack.
             * @memberof PB
             * @classdesc Represents an Ack.
             * @implements IAck
             * @constructor
             * @param {PB.IAck=} [properties] Properties to set
             */
            function Ack(properties) {
                if (properties)
                    for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                        if (properties[keys[i]] != null)
                            this[keys[i]] = properties[keys[i]];
            }
    
            /**
             * Ack nonce.
             * @member {number} nonce
             * @memberof PB.Ack
             * @instance
             */
            Ack.prototype.nonce = 0;
    
            /**
             * Creates a new Ack instance using the specified properties.
             * @function create
             * @memberof PB.Ack
             * @static
             * @param {PB.IAck=} [properties] Properties to set
             * @returns {PB.Ack} Ack instance
             */
            Ack.create = function create(properties) {
                return new Ack(properties);
            };
    
            /**
             * Encodes the specified Ack message. Does not implicitly {@link PB.Ack.verify|verify} messages.
             * @function encode
             * @memberof PB.Ack
             * @static
             * @param {PB.IAck} message Ack message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            Ack.encode = function encode(message, writer) {
                if (!writer)
                    writer = $Writer.create();
                if (message.nonce != null && Object.hasOwnProperty.call(message, "nonce"))
                    writer.uint32(/* id 1, wireType 0 =*/8).uint32(message.nonce);
                return writer;
            };
    
            /**
             * Encodes the specified Ack message, length delimited. Does not implicitly {@link PB.Ack.verify|verify} messages.
             * @function encodeDelimited
             * @memberof PB.Ack
             * @static
             * @param {PB.IAck} message Ack message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            Ack.encodeDelimited = function encodeDelimited(message, writer) {
                return this.encode(message, writer).ldelim();
            };
    
            /**
             * Decodes an Ack message from the specified reader or buffer.
             * @function decode
             * @memberof PB.Ack
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @param {number} [length] Message length if known beforehand
             * @returns {PB.Ack} Ack
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            Ack.decode = function decode(reader, length) {
                if (!(reader instanceof $Reader))
                    reader = $Reader.create(reader);
                var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.Ack();
                while (reader.pos < end) {
                    var tag = reader.uint32();
                    switch (tag >>> 3) {
                    case 1:
                        message.nonce = reader.uint32();
                        break;
                    default:
                        reader.skipType(tag & 7);
                        break;
                    }
                }
                return message;
            };
    
            /**
             * Decodes an Ack message from the specified reader or buffer, length delimited.
             * @function decodeDelimited
             * @memberof PB.Ack
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @returns {PB.Ack} Ack
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            Ack.decodeDelimited = function decodeDelimited(reader) {
                if (!(reader instanceof $Reader))
                    reader = new $Reader(reader);
                return this.decode(reader, reader.uint32());
            };
    
            /**
             * Verifies an Ack message.
             * @function verify
             * @memberof PB.Ack
             * @static
             * @param {Object.<string,*>} message Plain object to verify
             * @returns {string|null} `null` if valid, otherwise the reason why it is not
             */
            Ack.verify = function verify(message) {
                if (typeof message !== "object" || message === null)
                    return "object expected";
                if (message.nonce != null && message.hasOwnProperty("nonce"))
                    if (!$util.isInteger(message.nonce))
                        return "nonce: integer expected";
                return null;
            };
    
            /**
             * Creates an Ack message from a plain object. Also converts values to their respective internal types.
             * @function fromObject
             * @memberof PB.Ack
             * @static
             * @param {Object.<string,*>} object Plain object
             * @returns {PB.Ack} Ack
             */
            Ack.fromObject = function fromObject(object) {
                if (object instanceof $root.PB.Ack)
                    return object;
                var message = new $root.PB.Ack();
                if (object.nonce != null)
                    message.nonce = object.nonce >>> 0;
                return message;
            };
    
            /**
             * Creates a plain object from an Ack message. Also converts values to other types if specified.
             * @function toObject
             * @memberof PB.Ack
             * @static
             * @param {PB.Ack} message Ack
             * @param {$protobuf.IConversionOptions} [options] Conversion options
             * @returns {Object.<string,*>} Plain object
             */
            Ack.toObject = function toObject(message, options) {
                if (!options)
                    options = {};
                var object = {};
                if (options.defaults)
                    object.nonce = 0;
                if (message.nonce != null && message.hasOwnProperty("nonce"))
                    object.nonce = message.nonce;
                return object;
            };
    
            /**
             * Converts this Ack to JSON.
             * @function toJSON
             * @memberof PB.Ack
             * @instance
             * @returns {Object.<string,*>} JSON object
             */
            Ack.prototype.toJSON = function toJSON() {
                return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
            };
    
            return Ack;
        })();
    
        PB.SupervisorState = (function() {
    
            /**
             * Properties of a SupervisorState.
             * @memberof PB
             * @interface ISupervisorState
             * @property {number|null} [uptimeMillis] SupervisorState uptimeMillis
             * @property {PB.SupervisorState.State|null} [state] SupervisorState state
             * @property {Array.<PB.SupervisorState.IPowerChannelState>|null} [powerChannels] SupervisorState powerChannels
             * @property {PB.SupervisorState.IFaultInfo|null} [faultInfo] SupervisorState faultInfo
             */
    
            /**
             * Constructs a new SupervisorState.
             * @memberof PB
             * @classdesc Represents a SupervisorState.
             * @implements ISupervisorState
             * @constructor
             * @param {PB.ISupervisorState=} [properties] Properties to set
             */
            function SupervisorState(properties) {
                this.powerChannels = [];
                if (properties)
                    for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                        if (properties[keys[i]] != null)
                            this[keys[i]] = properties[keys[i]];
            }
    
            /**
             * SupervisorState uptimeMillis.
             * @member {number} uptimeMillis
             * @memberof PB.SupervisorState
             * @instance
             */
            SupervisorState.prototype.uptimeMillis = 0;
    
            /**
             * SupervisorState state.
             * @member {PB.SupervisorState.State} state
             * @memberof PB.SupervisorState
             * @instance
             */
            SupervisorState.prototype.state = 0;
    
            /**
             * SupervisorState powerChannels.
             * @member {Array.<PB.SupervisorState.IPowerChannelState>} powerChannels
             * @memberof PB.SupervisorState
             * @instance
             */
            SupervisorState.prototype.powerChannels = $util.emptyArray;
    
            /**
             * SupervisorState faultInfo.
             * @member {PB.SupervisorState.IFaultInfo|null|undefined} faultInfo
             * @memberof PB.SupervisorState
             * @instance
             */
            SupervisorState.prototype.faultInfo = null;
    
            /**
             * Creates a new SupervisorState instance using the specified properties.
             * @function create
             * @memberof PB.SupervisorState
             * @static
             * @param {PB.ISupervisorState=} [properties] Properties to set
             * @returns {PB.SupervisorState} SupervisorState instance
             */
            SupervisorState.create = function create(properties) {
                return new SupervisorState(properties);
            };
    
            /**
             * Encodes the specified SupervisorState message. Does not implicitly {@link PB.SupervisorState.verify|verify} messages.
             * @function encode
             * @memberof PB.SupervisorState
             * @static
             * @param {PB.ISupervisorState} message SupervisorState message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            SupervisorState.encode = function encode(message, writer) {
                if (!writer)
                    writer = $Writer.create();
                if (message.uptimeMillis != null && Object.hasOwnProperty.call(message, "uptimeMillis"))
                    writer.uint32(/* id 1, wireType 0 =*/8).uint32(message.uptimeMillis);
                if (message.state != null && Object.hasOwnProperty.call(message, "state"))
                    writer.uint32(/* id 2, wireType 0 =*/16).int32(message.state);
                if (message.powerChannels != null && message.powerChannels.length)
                    for (var i = 0; i < message.powerChannels.length; ++i)
                        $root.PB.SupervisorState.PowerChannelState.encode(message.powerChannels[i], writer.uint32(/* id 3, wireType 2 =*/26).fork()).ldelim();
                if (message.faultInfo != null && Object.hasOwnProperty.call(message, "faultInfo"))
                    $root.PB.SupervisorState.FaultInfo.encode(message.faultInfo, writer.uint32(/* id 4, wireType 2 =*/34).fork()).ldelim();
                return writer;
            };
    
            /**
             * Encodes the specified SupervisorState message, length delimited. Does not implicitly {@link PB.SupervisorState.verify|verify} messages.
             * @function encodeDelimited
             * @memberof PB.SupervisorState
             * @static
             * @param {PB.ISupervisorState} message SupervisorState message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            SupervisorState.encodeDelimited = function encodeDelimited(message, writer) {
                return this.encode(message, writer).ldelim();
            };
    
            /**
             * Decodes a SupervisorState message from the specified reader or buffer.
             * @function decode
             * @memberof PB.SupervisorState
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @param {number} [length] Message length if known beforehand
             * @returns {PB.SupervisorState} SupervisorState
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            SupervisorState.decode = function decode(reader, length) {
                if (!(reader instanceof $Reader))
                    reader = $Reader.create(reader);
                var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.SupervisorState();
                while (reader.pos < end) {
                    var tag = reader.uint32();
                    switch (tag >>> 3) {
                    case 1:
                        message.uptimeMillis = reader.uint32();
                        break;
                    case 2:
                        message.state = reader.int32();
                        break;
                    case 3:
                        if (!(message.powerChannels && message.powerChannels.length))
                            message.powerChannels = [];
                        message.powerChannels.push($root.PB.SupervisorState.PowerChannelState.decode(reader, reader.uint32()));
                        break;
                    case 4:
                        message.faultInfo = $root.PB.SupervisorState.FaultInfo.decode(reader, reader.uint32());
                        break;
                    default:
                        reader.skipType(tag & 7);
                        break;
                    }
                }
                return message;
            };
    
            /**
             * Decodes a SupervisorState message from the specified reader or buffer, length delimited.
             * @function decodeDelimited
             * @memberof PB.SupervisorState
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @returns {PB.SupervisorState} SupervisorState
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            SupervisorState.decodeDelimited = function decodeDelimited(reader) {
                if (!(reader instanceof $Reader))
                    reader = new $Reader(reader);
                return this.decode(reader, reader.uint32());
            };
    
            /**
             * Verifies a SupervisorState message.
             * @function verify
             * @memberof PB.SupervisorState
             * @static
             * @param {Object.<string,*>} message Plain object to verify
             * @returns {string|null} `null` if valid, otherwise the reason why it is not
             */
            SupervisorState.verify = function verify(message) {
                if (typeof message !== "object" || message === null)
                    return "object expected";
                if (message.uptimeMillis != null && message.hasOwnProperty("uptimeMillis"))
                    if (!$util.isInteger(message.uptimeMillis))
                        return "uptimeMillis: integer expected";
                if (message.state != null && message.hasOwnProperty("state"))
                    switch (message.state) {
                    default:
                        return "state: enum value expected";
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        break;
                    }
                if (message.powerChannels != null && message.hasOwnProperty("powerChannels")) {
                    if (!Array.isArray(message.powerChannels))
                        return "powerChannels: array expected";
                    for (var i = 0; i < message.powerChannels.length; ++i) {
                        var error = $root.PB.SupervisorState.PowerChannelState.verify(message.powerChannels[i]);
                        if (error)
                            return "powerChannels." + error;
                    }
                }
                if (message.faultInfo != null && message.hasOwnProperty("faultInfo")) {
                    var error = $root.PB.SupervisorState.FaultInfo.verify(message.faultInfo);
                    if (error)
                        return "faultInfo." + error;
                }
                return null;
            };
    
            /**
             * Creates a SupervisorState message from a plain object. Also converts values to their respective internal types.
             * @function fromObject
             * @memberof PB.SupervisorState
             * @static
             * @param {Object.<string,*>} object Plain object
             * @returns {PB.SupervisorState} SupervisorState
             */
            SupervisorState.fromObject = function fromObject(object) {
                if (object instanceof $root.PB.SupervisorState)
                    return object;
                var message = new $root.PB.SupervisorState();
                if (object.uptimeMillis != null)
                    message.uptimeMillis = object.uptimeMillis >>> 0;
                switch (object.state) {
                case "UNKNOWN":
                case 0:
                    message.state = 0;
                    break;
                case "STARTING_VERIFY_PSU_OFF":
                case 1:
                    message.state = 1;
                    break;
                case "STARTING_VERIFY_VOLTAGES":
                case 2:
                    message.state = 2;
                    break;
                case "STARTING_ENABLE_CHANNELS":
                case 3:
                    message.state = 3;
                    break;
                case "NORMAL":
                case 4:
                    message.state = 4;
                    break;
                case "FAULT":
                case 5:
                    message.state = 5;
                    break;
                }
                if (object.powerChannels) {
                    if (!Array.isArray(object.powerChannels))
                        throw TypeError(".PB.SupervisorState.powerChannels: array expected");
                    message.powerChannels = [];
                    for (var i = 0; i < object.powerChannels.length; ++i) {
                        if (typeof object.powerChannels[i] !== "object")
                            throw TypeError(".PB.SupervisorState.powerChannels: object expected");
                        message.powerChannels[i] = $root.PB.SupervisorState.PowerChannelState.fromObject(object.powerChannels[i]);
                    }
                }
                if (object.faultInfo != null) {
                    if (typeof object.faultInfo !== "object")
                        throw TypeError(".PB.SupervisorState.faultInfo: object expected");
                    message.faultInfo = $root.PB.SupervisorState.FaultInfo.fromObject(object.faultInfo);
                }
                return message;
            };
    
            /**
             * Creates a plain object from a SupervisorState message. Also converts values to other types if specified.
             * @function toObject
             * @memberof PB.SupervisorState
             * @static
             * @param {PB.SupervisorState} message SupervisorState
             * @param {$protobuf.IConversionOptions} [options] Conversion options
             * @returns {Object.<string,*>} Plain object
             */
            SupervisorState.toObject = function toObject(message, options) {
                if (!options)
                    options = {};
                var object = {};
                if (options.arrays || options.defaults)
                    object.powerChannels = [];
                if (options.defaults) {
                    object.uptimeMillis = 0;
                    object.state = options.enums === String ? "UNKNOWN" : 0;
                    object.faultInfo = null;
                }
                if (message.uptimeMillis != null && message.hasOwnProperty("uptimeMillis"))
                    object.uptimeMillis = message.uptimeMillis;
                if (message.state != null && message.hasOwnProperty("state"))
                    object.state = options.enums === String ? $root.PB.SupervisorState.State[message.state] : message.state;
                if (message.powerChannels && message.powerChannels.length) {
                    object.powerChannels = [];
                    for (var j = 0; j < message.powerChannels.length; ++j)
                        object.powerChannels[j] = $root.PB.SupervisorState.PowerChannelState.toObject(message.powerChannels[j], options);
                }
                if (message.faultInfo != null && message.hasOwnProperty("faultInfo"))
                    object.faultInfo = $root.PB.SupervisorState.FaultInfo.toObject(message.faultInfo, options);
                return object;
            };
    
            /**
             * Converts this SupervisorState to JSON.
             * @function toJSON
             * @memberof PB.SupervisorState
             * @instance
             * @returns {Object.<string,*>} JSON object
             */
            SupervisorState.prototype.toJSON = function toJSON() {
                return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
            };
    
            /**
             * State enum.
             * @name PB.SupervisorState.State
             * @enum {number}
             * @property {number} UNKNOWN=0 UNKNOWN value
             * @property {number} STARTING_VERIFY_PSU_OFF=1 STARTING_VERIFY_PSU_OFF value
             * @property {number} STARTING_VERIFY_VOLTAGES=2 STARTING_VERIFY_VOLTAGES value
             * @property {number} STARTING_ENABLE_CHANNELS=3 STARTING_ENABLE_CHANNELS value
             * @property {number} NORMAL=4 NORMAL value
             * @property {number} FAULT=5 FAULT value
             */
            SupervisorState.State = (function() {
                var valuesById = {}, values = Object.create(valuesById);
                values[valuesById[0] = "UNKNOWN"] = 0;
                values[valuesById[1] = "STARTING_VERIFY_PSU_OFF"] = 1;
                values[valuesById[2] = "STARTING_VERIFY_VOLTAGES"] = 2;
                values[valuesById[3] = "STARTING_ENABLE_CHANNELS"] = 3;
                values[valuesById[4] = "NORMAL"] = 4;
                values[valuesById[5] = "FAULT"] = 5;
                return values;
            })();
    
            SupervisorState.PowerChannelState = (function() {
    
                /**
                 * Properties of a PowerChannelState.
                 * @memberof PB.SupervisorState
                 * @interface IPowerChannelState
                 * @property {number|null} [voltageVolts] PowerChannelState voltageVolts
                 * @property {number|null} [currentAmps] PowerChannelState currentAmps
                 * @property {boolean|null} [on] PowerChannelState on
                 */
    
                /**
                 * Constructs a new PowerChannelState.
                 * @memberof PB.SupervisorState
                 * @classdesc Represents a PowerChannelState.
                 * @implements IPowerChannelState
                 * @constructor
                 * @param {PB.SupervisorState.IPowerChannelState=} [properties] Properties to set
                 */
                function PowerChannelState(properties) {
                    if (properties)
                        for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                            if (properties[keys[i]] != null)
                                this[keys[i]] = properties[keys[i]];
                }
    
                /**
                 * PowerChannelState voltageVolts.
                 * @member {number} voltageVolts
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @instance
                 */
                PowerChannelState.prototype.voltageVolts = 0;
    
                /**
                 * PowerChannelState currentAmps.
                 * @member {number} currentAmps
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @instance
                 */
                PowerChannelState.prototype.currentAmps = 0;
    
                /**
                 * PowerChannelState on.
                 * @member {boolean} on
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @instance
                 */
                PowerChannelState.prototype.on = false;
    
                /**
                 * Creates a new PowerChannelState instance using the specified properties.
                 * @function create
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @static
                 * @param {PB.SupervisorState.IPowerChannelState=} [properties] Properties to set
                 * @returns {PB.SupervisorState.PowerChannelState} PowerChannelState instance
                 */
                PowerChannelState.create = function create(properties) {
                    return new PowerChannelState(properties);
                };
    
                /**
                 * Encodes the specified PowerChannelState message. Does not implicitly {@link PB.SupervisorState.PowerChannelState.verify|verify} messages.
                 * @function encode
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @static
                 * @param {PB.SupervisorState.IPowerChannelState} message PowerChannelState message or plain object to encode
                 * @param {$protobuf.Writer} [writer] Writer to encode to
                 * @returns {$protobuf.Writer} Writer
                 */
                PowerChannelState.encode = function encode(message, writer) {
                    if (!writer)
                        writer = $Writer.create();
                    if (message.voltageVolts != null && Object.hasOwnProperty.call(message, "voltageVolts"))
                        writer.uint32(/* id 1, wireType 5 =*/13).float(message.voltageVolts);
                    if (message.currentAmps != null && Object.hasOwnProperty.call(message, "currentAmps"))
                        writer.uint32(/* id 2, wireType 5 =*/21).float(message.currentAmps);
                    if (message.on != null && Object.hasOwnProperty.call(message, "on"))
                        writer.uint32(/* id 3, wireType 0 =*/24).bool(message.on);
                    return writer;
                };
    
                /**
                 * Encodes the specified PowerChannelState message, length delimited. Does not implicitly {@link PB.SupervisorState.PowerChannelState.verify|verify} messages.
                 * @function encodeDelimited
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @static
                 * @param {PB.SupervisorState.IPowerChannelState} message PowerChannelState message or plain object to encode
                 * @param {$protobuf.Writer} [writer] Writer to encode to
                 * @returns {$protobuf.Writer} Writer
                 */
                PowerChannelState.encodeDelimited = function encodeDelimited(message, writer) {
                    return this.encode(message, writer).ldelim();
                };
    
                /**
                 * Decodes a PowerChannelState message from the specified reader or buffer.
                 * @function decode
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @static
                 * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
                 * @param {number} [length] Message length if known beforehand
                 * @returns {PB.SupervisorState.PowerChannelState} PowerChannelState
                 * @throws {Error} If the payload is not a reader or valid buffer
                 * @throws {$protobuf.util.ProtocolError} If required fields are missing
                 */
                PowerChannelState.decode = function decode(reader, length) {
                    if (!(reader instanceof $Reader))
                        reader = $Reader.create(reader);
                    var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.SupervisorState.PowerChannelState();
                    while (reader.pos < end) {
                        var tag = reader.uint32();
                        switch (tag >>> 3) {
                        case 1:
                            message.voltageVolts = reader.float();
                            break;
                        case 2:
                            message.currentAmps = reader.float();
                            break;
                        case 3:
                            message.on = reader.bool();
                            break;
                        default:
                            reader.skipType(tag & 7);
                            break;
                        }
                    }
                    return message;
                };
    
                /**
                 * Decodes a PowerChannelState message from the specified reader or buffer, length delimited.
                 * @function decodeDelimited
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @static
                 * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
                 * @returns {PB.SupervisorState.PowerChannelState} PowerChannelState
                 * @throws {Error} If the payload is not a reader or valid buffer
                 * @throws {$protobuf.util.ProtocolError} If required fields are missing
                 */
                PowerChannelState.decodeDelimited = function decodeDelimited(reader) {
                    if (!(reader instanceof $Reader))
                        reader = new $Reader(reader);
                    return this.decode(reader, reader.uint32());
                };
    
                /**
                 * Verifies a PowerChannelState message.
                 * @function verify
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @static
                 * @param {Object.<string,*>} message Plain object to verify
                 * @returns {string|null} `null` if valid, otherwise the reason why it is not
                 */
                PowerChannelState.verify = function verify(message) {
                    if (typeof message !== "object" || message === null)
                        return "object expected";
                    if (message.voltageVolts != null && message.hasOwnProperty("voltageVolts"))
                        if (typeof message.voltageVolts !== "number")
                            return "voltageVolts: number expected";
                    if (message.currentAmps != null && message.hasOwnProperty("currentAmps"))
                        if (typeof message.currentAmps !== "number")
                            return "currentAmps: number expected";
                    if (message.on != null && message.hasOwnProperty("on"))
                        if (typeof message.on !== "boolean")
                            return "on: boolean expected";
                    return null;
                };
    
                /**
                 * Creates a PowerChannelState message from a plain object. Also converts values to their respective internal types.
                 * @function fromObject
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @static
                 * @param {Object.<string,*>} object Plain object
                 * @returns {PB.SupervisorState.PowerChannelState} PowerChannelState
                 */
                PowerChannelState.fromObject = function fromObject(object) {
                    if (object instanceof $root.PB.SupervisorState.PowerChannelState)
                        return object;
                    var message = new $root.PB.SupervisorState.PowerChannelState();
                    if (object.voltageVolts != null)
                        message.voltageVolts = Number(object.voltageVolts);
                    if (object.currentAmps != null)
                        message.currentAmps = Number(object.currentAmps);
                    if (object.on != null)
                        message.on = Boolean(object.on);
                    return message;
                };
    
                /**
                 * Creates a plain object from a PowerChannelState message. Also converts values to other types if specified.
                 * @function toObject
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @static
                 * @param {PB.SupervisorState.PowerChannelState} message PowerChannelState
                 * @param {$protobuf.IConversionOptions} [options] Conversion options
                 * @returns {Object.<string,*>} Plain object
                 */
                PowerChannelState.toObject = function toObject(message, options) {
                    if (!options)
                        options = {};
                    var object = {};
                    if (options.defaults) {
                        object.voltageVolts = 0;
                        object.currentAmps = 0;
                        object.on = false;
                    }
                    if (message.voltageVolts != null && message.hasOwnProperty("voltageVolts"))
                        object.voltageVolts = options.json && !isFinite(message.voltageVolts) ? String(message.voltageVolts) : message.voltageVolts;
                    if (message.currentAmps != null && message.hasOwnProperty("currentAmps"))
                        object.currentAmps = options.json && !isFinite(message.currentAmps) ? String(message.currentAmps) : message.currentAmps;
                    if (message.on != null && message.hasOwnProperty("on"))
                        object.on = message.on;
                    return object;
                };
    
                /**
                 * Converts this PowerChannelState to JSON.
                 * @function toJSON
                 * @memberof PB.SupervisorState.PowerChannelState
                 * @instance
                 * @returns {Object.<string,*>} JSON object
                 */
                PowerChannelState.prototype.toJSON = function toJSON() {
                    return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
                };
    
                return PowerChannelState;
            })();
    
            SupervisorState.FaultInfo = (function() {
    
                /**
                 * Properties of a FaultInfo.
                 * @memberof PB.SupervisorState
                 * @interface IFaultInfo
                 * @property {PB.SupervisorState.FaultInfo.FaultType|null} [type] FaultInfo type
                 * @property {string|null} [msg] FaultInfo msg
                 * @property {number|null} [tsMillis] FaultInfo tsMillis
                 */
    
                /**
                 * Constructs a new FaultInfo.
                 * @memberof PB.SupervisorState
                 * @classdesc Represents a FaultInfo.
                 * @implements IFaultInfo
                 * @constructor
                 * @param {PB.SupervisorState.IFaultInfo=} [properties] Properties to set
                 */
                function FaultInfo(properties) {
                    if (properties)
                        for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                            if (properties[keys[i]] != null)
                                this[keys[i]] = properties[keys[i]];
                }
    
                /**
                 * FaultInfo type.
                 * @member {PB.SupervisorState.FaultInfo.FaultType} type
                 * @memberof PB.SupervisorState.FaultInfo
                 * @instance
                 */
                FaultInfo.prototype.type = 0;
    
                /**
                 * FaultInfo msg.
                 * @member {string} msg
                 * @memberof PB.SupervisorState.FaultInfo
                 * @instance
                 */
                FaultInfo.prototype.msg = "";
    
                /**
                 * FaultInfo tsMillis.
                 * @member {number} tsMillis
                 * @memberof PB.SupervisorState.FaultInfo
                 * @instance
                 */
                FaultInfo.prototype.tsMillis = 0;
    
                /**
                 * Creates a new FaultInfo instance using the specified properties.
                 * @function create
                 * @memberof PB.SupervisorState.FaultInfo
                 * @static
                 * @param {PB.SupervisorState.IFaultInfo=} [properties] Properties to set
                 * @returns {PB.SupervisorState.FaultInfo} FaultInfo instance
                 */
                FaultInfo.create = function create(properties) {
                    return new FaultInfo(properties);
                };
    
                /**
                 * Encodes the specified FaultInfo message. Does not implicitly {@link PB.SupervisorState.FaultInfo.verify|verify} messages.
                 * @function encode
                 * @memberof PB.SupervisorState.FaultInfo
                 * @static
                 * @param {PB.SupervisorState.IFaultInfo} message FaultInfo message or plain object to encode
                 * @param {$protobuf.Writer} [writer] Writer to encode to
                 * @returns {$protobuf.Writer} Writer
                 */
                FaultInfo.encode = function encode(message, writer) {
                    if (!writer)
                        writer = $Writer.create();
                    if (message.type != null && Object.hasOwnProperty.call(message, "type"))
                        writer.uint32(/* id 1, wireType 0 =*/8).int32(message.type);
                    if (message.msg != null && Object.hasOwnProperty.call(message, "msg"))
                        writer.uint32(/* id 2, wireType 2 =*/18).string(message.msg);
                    if (message.tsMillis != null && Object.hasOwnProperty.call(message, "tsMillis"))
                        writer.uint32(/* id 3, wireType 0 =*/24).uint32(message.tsMillis);
                    return writer;
                };
    
                /**
                 * Encodes the specified FaultInfo message, length delimited. Does not implicitly {@link PB.SupervisorState.FaultInfo.verify|verify} messages.
                 * @function encodeDelimited
                 * @memberof PB.SupervisorState.FaultInfo
                 * @static
                 * @param {PB.SupervisorState.IFaultInfo} message FaultInfo message or plain object to encode
                 * @param {$protobuf.Writer} [writer] Writer to encode to
                 * @returns {$protobuf.Writer} Writer
                 */
                FaultInfo.encodeDelimited = function encodeDelimited(message, writer) {
                    return this.encode(message, writer).ldelim();
                };
    
                /**
                 * Decodes a FaultInfo message from the specified reader or buffer.
                 * @function decode
                 * @memberof PB.SupervisorState.FaultInfo
                 * @static
                 * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
                 * @param {number} [length] Message length if known beforehand
                 * @returns {PB.SupervisorState.FaultInfo} FaultInfo
                 * @throws {Error} If the payload is not a reader or valid buffer
                 * @throws {$protobuf.util.ProtocolError} If required fields are missing
                 */
                FaultInfo.decode = function decode(reader, length) {
                    if (!(reader instanceof $Reader))
                        reader = $Reader.create(reader);
                    var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.SupervisorState.FaultInfo();
                    while (reader.pos < end) {
                        var tag = reader.uint32();
                        switch (tag >>> 3) {
                        case 1:
                            message.type = reader.int32();
                            break;
                        case 2:
                            message.msg = reader.string();
                            break;
                        case 3:
                            message.tsMillis = reader.uint32();
                            break;
                        default:
                            reader.skipType(tag & 7);
                            break;
                        }
                    }
                    return message;
                };
    
                /**
                 * Decodes a FaultInfo message from the specified reader or buffer, length delimited.
                 * @function decodeDelimited
                 * @memberof PB.SupervisorState.FaultInfo
                 * @static
                 * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
                 * @returns {PB.SupervisorState.FaultInfo} FaultInfo
                 * @throws {Error} If the payload is not a reader or valid buffer
                 * @throws {$protobuf.util.ProtocolError} If required fields are missing
                 */
                FaultInfo.decodeDelimited = function decodeDelimited(reader) {
                    if (!(reader instanceof $Reader))
                        reader = new $Reader(reader);
                    return this.decode(reader, reader.uint32());
                };
    
                /**
                 * Verifies a FaultInfo message.
                 * @function verify
                 * @memberof PB.SupervisorState.FaultInfo
                 * @static
                 * @param {Object.<string,*>} message Plain object to verify
                 * @returns {string|null} `null` if valid, otherwise the reason why it is not
                 */
                FaultInfo.verify = function verify(message) {
                    if (typeof message !== "object" || message === null)
                        return "object expected";
                    if (message.type != null && message.hasOwnProperty("type"))
                        switch (message.type) {
                        default:
                            return "type: enum value expected";
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 6:
                            break;
                        }
                    if (message.msg != null && message.hasOwnProperty("msg"))
                        if (!$util.isString(message.msg))
                            return "msg: string expected";
                    if (message.tsMillis != null && message.hasOwnProperty("tsMillis"))
                        if (!$util.isInteger(message.tsMillis))
                            return "tsMillis: integer expected";
                    return null;
                };
    
                /**
                 * Creates a FaultInfo message from a plain object. Also converts values to their respective internal types.
                 * @function fromObject
                 * @memberof PB.SupervisorState.FaultInfo
                 * @static
                 * @param {Object.<string,*>} object Plain object
                 * @returns {PB.SupervisorState.FaultInfo} FaultInfo
                 */
                FaultInfo.fromObject = function fromObject(object) {
                    if (object instanceof $root.PB.SupervisorState.FaultInfo)
                        return object;
                    var message = new $root.PB.SupervisorState.FaultInfo();
                    switch (object.type) {
                    case "UNKNOWN":
                    case 0:
                        message.type = 0;
                        break;
                    case "NONE":
                    case 1:
                        message.type = 1;
                        break;
                    case "INRUSH_CURRENT_NOT_SETTLED":
                    case 2:
                        message.type = 2;
                        break;
                    case "SPLITFLAP_SHUTDOWN":
                    case 3:
                        message.type = 3;
                        break;
                    case "OUT_OF_RANGE":
                    case 4:
                        message.type = 4;
                        break;
                    case "OVER_CURRENT":
                    case 5:
                        message.type = 5;
                        break;
                    case "UNEXPECTED_POWER":
                    case 6:
                        message.type = 6;
                        break;
                    }
                    if (object.msg != null)
                        message.msg = String(object.msg);
                    if (object.tsMillis != null)
                        message.tsMillis = object.tsMillis >>> 0;
                    return message;
                };
    
                /**
                 * Creates a plain object from a FaultInfo message. Also converts values to other types if specified.
                 * @function toObject
                 * @memberof PB.SupervisorState.FaultInfo
                 * @static
                 * @param {PB.SupervisorState.FaultInfo} message FaultInfo
                 * @param {$protobuf.IConversionOptions} [options] Conversion options
                 * @returns {Object.<string,*>} Plain object
                 */
                FaultInfo.toObject = function toObject(message, options) {
                    if (!options)
                        options = {};
                    var object = {};
                    if (options.defaults) {
                        object.type = options.enums === String ? "UNKNOWN" : 0;
                        object.msg = "";
                        object.tsMillis = 0;
                    }
                    if (message.type != null && message.hasOwnProperty("type"))
                        object.type = options.enums === String ? $root.PB.SupervisorState.FaultInfo.FaultType[message.type] : message.type;
                    if (message.msg != null && message.hasOwnProperty("msg"))
                        object.msg = message.msg;
                    if (message.tsMillis != null && message.hasOwnProperty("tsMillis"))
                        object.tsMillis = message.tsMillis;
                    return object;
                };
    
                /**
                 * Converts this FaultInfo to JSON.
                 * @function toJSON
                 * @memberof PB.SupervisorState.FaultInfo
                 * @instance
                 * @returns {Object.<string,*>} JSON object
                 */
                FaultInfo.prototype.toJSON = function toJSON() {
                    return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
                };
    
                /**
                 * FaultType enum.
                 * @name PB.SupervisorState.FaultInfo.FaultType
                 * @enum {number}
                 * @property {number} UNKNOWN=0 UNKNOWN value
                 * @property {number} NONE=1 NONE value
                 * @property {number} INRUSH_CURRENT_NOT_SETTLED=2 INRUSH_CURRENT_NOT_SETTLED value
                 * @property {number} SPLITFLAP_SHUTDOWN=3 SPLITFLAP_SHUTDOWN value
                 * @property {number} OUT_OF_RANGE=4 OUT_OF_RANGE value
                 * @property {number} OVER_CURRENT=5 OVER_CURRENT value
                 * @property {number} UNEXPECTED_POWER=6 UNEXPECTED_POWER value
                 */
                FaultInfo.FaultType = (function() {
                    var valuesById = {}, values = Object.create(valuesById);
                    values[valuesById[0] = "UNKNOWN"] = 0;
                    values[valuesById[1] = "NONE"] = 1;
                    values[valuesById[2] = "INRUSH_CURRENT_NOT_SETTLED"] = 2;
                    values[valuesById[3] = "SPLITFLAP_SHUTDOWN"] = 3;
                    values[valuesById[4] = "OUT_OF_RANGE"] = 4;
                    values[valuesById[5] = "OVER_CURRENT"] = 5;
                    values[valuesById[6] = "UNEXPECTED_POWER"] = 6;
                    return values;
                })();
    
                return FaultInfo;
            })();
    
            return SupervisorState;
        })();
    
        PB.FromSplitflap = (function() {
    
            /**
             * Properties of a FromSplitflap.
             * @memberof PB
             * @interface IFromSplitflap
             * @property {PB.ISplitflapState|null} [splitflapState] FromSplitflap splitflapState
             * @property {PB.ILog|null} [log] FromSplitflap log
             * @property {PB.IAck|null} [ack] FromSplitflap ack
             * @property {PB.ISupervisorState|null} [supervisorState] FromSplitflap supervisorState
             */
    
            /**
             * Constructs a new FromSplitflap.
             * @memberof PB
             * @classdesc Represents a FromSplitflap.
             * @implements IFromSplitflap
             * @constructor
             * @param {PB.IFromSplitflap=} [properties] Properties to set
             */
            function FromSplitflap(properties) {
                if (properties)
                    for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                        if (properties[keys[i]] != null)
                            this[keys[i]] = properties[keys[i]];
            }
    
            /**
             * FromSplitflap splitflapState.
             * @member {PB.ISplitflapState|null|undefined} splitflapState
             * @memberof PB.FromSplitflap
             * @instance
             */
            FromSplitflap.prototype.splitflapState = null;
    
            /**
             * FromSplitflap log.
             * @member {PB.ILog|null|undefined} log
             * @memberof PB.FromSplitflap
             * @instance
             */
            FromSplitflap.prototype.log = null;
    
            /**
             * FromSplitflap ack.
             * @member {PB.IAck|null|undefined} ack
             * @memberof PB.FromSplitflap
             * @instance
             */
            FromSplitflap.prototype.ack = null;
    
            /**
             * FromSplitflap supervisorState.
             * @member {PB.ISupervisorState|null|undefined} supervisorState
             * @memberof PB.FromSplitflap
             * @instance
             */
            FromSplitflap.prototype.supervisorState = null;
    
            // OneOf field names bound to virtual getters and setters
            var $oneOfFields;
    
            /**
             * FromSplitflap payload.
             * @member {"splitflapState"|"log"|"ack"|"supervisorState"|undefined} payload
             * @memberof PB.FromSplitflap
             * @instance
             */
            Object.defineProperty(FromSplitflap.prototype, "payload", {
                get: $util.oneOfGetter($oneOfFields = ["splitflapState", "log", "ack", "supervisorState"]),
                set: $util.oneOfSetter($oneOfFields)
            });
    
            /**
             * Creates a new FromSplitflap instance using the specified properties.
             * @function create
             * @memberof PB.FromSplitflap
             * @static
             * @param {PB.IFromSplitflap=} [properties] Properties to set
             * @returns {PB.FromSplitflap} FromSplitflap instance
             */
            FromSplitflap.create = function create(properties) {
                return new FromSplitflap(properties);
            };
    
            /**
             * Encodes the specified FromSplitflap message. Does not implicitly {@link PB.FromSplitflap.verify|verify} messages.
             * @function encode
             * @memberof PB.FromSplitflap
             * @static
             * @param {PB.IFromSplitflap} message FromSplitflap message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            FromSplitflap.encode = function encode(message, writer) {
                if (!writer)
                    writer = $Writer.create();
                if (message.splitflapState != null && Object.hasOwnProperty.call(message, "splitflapState"))
                    $root.PB.SplitflapState.encode(message.splitflapState, writer.uint32(/* id 1, wireType 2 =*/10).fork()).ldelim();
                if (message.log != null && Object.hasOwnProperty.call(message, "log"))
                    $root.PB.Log.encode(message.log, writer.uint32(/* id 2, wireType 2 =*/18).fork()).ldelim();
                if (message.ack != null && Object.hasOwnProperty.call(message, "ack"))
                    $root.PB.Ack.encode(message.ack, writer.uint32(/* id 3, wireType 2 =*/26).fork()).ldelim();
                if (message.supervisorState != null && Object.hasOwnProperty.call(message, "supervisorState"))
                    $root.PB.SupervisorState.encode(message.supervisorState, writer.uint32(/* id 4, wireType 2 =*/34).fork()).ldelim();
                return writer;
            };
    
            /**
             * Encodes the specified FromSplitflap message, length delimited. Does not implicitly {@link PB.FromSplitflap.verify|verify} messages.
             * @function encodeDelimited
             * @memberof PB.FromSplitflap
             * @static
             * @param {PB.IFromSplitflap} message FromSplitflap message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            FromSplitflap.encodeDelimited = function encodeDelimited(message, writer) {
                return this.encode(message, writer).ldelim();
            };
    
            /**
             * Decodes a FromSplitflap message from the specified reader or buffer.
             * @function decode
             * @memberof PB.FromSplitflap
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @param {number} [length] Message length if known beforehand
             * @returns {PB.FromSplitflap} FromSplitflap
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            FromSplitflap.decode = function decode(reader, length) {
                if (!(reader instanceof $Reader))
                    reader = $Reader.create(reader);
                var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.FromSplitflap();
                while (reader.pos < end) {
                    var tag = reader.uint32();
                    switch (tag >>> 3) {
                    case 1:
                        message.splitflapState = $root.PB.SplitflapState.decode(reader, reader.uint32());
                        break;
                    case 2:
                        message.log = $root.PB.Log.decode(reader, reader.uint32());
                        break;
                    case 3:
                        message.ack = $root.PB.Ack.decode(reader, reader.uint32());
                        break;
                    case 4:
                        message.supervisorState = $root.PB.SupervisorState.decode(reader, reader.uint32());
                        break;
                    default:
                        reader.skipType(tag & 7);
                        break;
                    }
                }
                return message;
            };
    
            /**
             * Decodes a FromSplitflap message from the specified reader or buffer, length delimited.
             * @function decodeDelimited
             * @memberof PB.FromSplitflap
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @returns {PB.FromSplitflap} FromSplitflap
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            FromSplitflap.decodeDelimited = function decodeDelimited(reader) {
                if (!(reader instanceof $Reader))
                    reader = new $Reader(reader);
                return this.decode(reader, reader.uint32());
            };
    
            /**
             * Verifies a FromSplitflap message.
             * @function verify
             * @memberof PB.FromSplitflap
             * @static
             * @param {Object.<string,*>} message Plain object to verify
             * @returns {string|null} `null` if valid, otherwise the reason why it is not
             */
            FromSplitflap.verify = function verify(message) {
                if (typeof message !== "object" || message === null)
                    return "object expected";
                var properties = {};
                if (message.splitflapState != null && message.hasOwnProperty("splitflapState")) {
                    properties.payload = 1;
                    {
                        var error = $root.PB.SplitflapState.verify(message.splitflapState);
                        if (error)
                            return "splitflapState." + error;
                    }
                }
                if (message.log != null && message.hasOwnProperty("log")) {
                    if (properties.payload === 1)
                        return "payload: multiple values";
                    properties.payload = 1;
                    {
                        var error = $root.PB.Log.verify(message.log);
                        if (error)
                            return "log." + error;
                    }
                }
                if (message.ack != null && message.hasOwnProperty("ack")) {
                    if (properties.payload === 1)
                        return "payload: multiple values";
                    properties.payload = 1;
                    {
                        var error = $root.PB.Ack.verify(message.ack);
                        if (error)
                            return "ack." + error;
                    }
                }
                if (message.supervisorState != null && message.hasOwnProperty("supervisorState")) {
                    if (properties.payload === 1)
                        return "payload: multiple values";
                    properties.payload = 1;
                    {
                        var error = $root.PB.SupervisorState.verify(message.supervisorState);
                        if (error)
                            return "supervisorState." + error;
                    }
                }
                return null;
            };
    
            /**
             * Creates a FromSplitflap message from a plain object. Also converts values to their respective internal types.
             * @function fromObject
             * @memberof PB.FromSplitflap
             * @static
             * @param {Object.<string,*>} object Plain object
             * @returns {PB.FromSplitflap} FromSplitflap
             */
            FromSplitflap.fromObject = function fromObject(object) {
                if (object instanceof $root.PB.FromSplitflap)
                    return object;
                var message = new $root.PB.FromSplitflap();
                if (object.splitflapState != null) {
                    if (typeof object.splitflapState !== "object")
                        throw TypeError(".PB.FromSplitflap.splitflapState: object expected");
                    message.splitflapState = $root.PB.SplitflapState.fromObject(object.splitflapState);
                }
                if (object.log != null) {
                    if (typeof object.log !== "object")
                        throw TypeError(".PB.FromSplitflap.log: object expected");
                    message.log = $root.PB.Log.fromObject(object.log);
                }
                if (object.ack != null) {
                    if (typeof object.ack !== "object")
                        throw TypeError(".PB.FromSplitflap.ack: object expected");
                    message.ack = $root.PB.Ack.fromObject(object.ack);
                }
                if (object.supervisorState != null) {
                    if (typeof object.supervisorState !== "object")
                        throw TypeError(".PB.FromSplitflap.supervisorState: object expected");
                    message.supervisorState = $root.PB.SupervisorState.fromObject(object.supervisorState);
                }
                return message;
            };
    
            /**
             * Creates a plain object from a FromSplitflap message. Also converts values to other types if specified.
             * @function toObject
             * @memberof PB.FromSplitflap
             * @static
             * @param {PB.FromSplitflap} message FromSplitflap
             * @param {$protobuf.IConversionOptions} [options] Conversion options
             * @returns {Object.<string,*>} Plain object
             */
            FromSplitflap.toObject = function toObject(message, options) {
                if (!options)
                    options = {};
                var object = {};
                if (message.splitflapState != null && message.hasOwnProperty("splitflapState")) {
                    object.splitflapState = $root.PB.SplitflapState.toObject(message.splitflapState, options);
                    if (options.oneofs)
                        object.payload = "splitflapState";
                }
                if (message.log != null && message.hasOwnProperty("log")) {
                    object.log = $root.PB.Log.toObject(message.log, options);
                    if (options.oneofs)
                        object.payload = "log";
                }
                if (message.ack != null && message.hasOwnProperty("ack")) {
                    object.ack = $root.PB.Ack.toObject(message.ack, options);
                    if (options.oneofs)
                        object.payload = "ack";
                }
                if (message.supervisorState != null && message.hasOwnProperty("supervisorState")) {
                    object.supervisorState = $root.PB.SupervisorState.toObject(message.supervisorState, options);
                    if (options.oneofs)
                        object.payload = "supervisorState";
                }
                return object;
            };
    
            /**
             * Converts this FromSplitflap to JSON.
             * @function toJSON
             * @memberof PB.FromSplitflap
             * @instance
             * @returns {Object.<string,*>} JSON object
             */
            FromSplitflap.prototype.toJSON = function toJSON() {
                return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
            };
    
            return FromSplitflap;
        })();
    
        PB.SplitflapCommand = (function() {
    
            /**
             * Properties of a SplitflapCommand.
             * @memberof PB
             * @interface ISplitflapCommand
             * @property {Array.<PB.SplitflapCommand.IModuleCommand>|null} [modules] SplitflapCommand modules
             */
    
            /**
             * Constructs a new SplitflapCommand.
             * @memberof PB
             * @classdesc Represents a SplitflapCommand.
             * @implements ISplitflapCommand
             * @constructor
             * @param {PB.ISplitflapCommand=} [properties] Properties to set
             */
            function SplitflapCommand(properties) {
                this.modules = [];
                if (properties)
                    for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                        if (properties[keys[i]] != null)
                            this[keys[i]] = properties[keys[i]];
            }
    
            /**
             * SplitflapCommand modules.
             * @member {Array.<PB.SplitflapCommand.IModuleCommand>} modules
             * @memberof PB.SplitflapCommand
             * @instance
             */
            SplitflapCommand.prototype.modules = $util.emptyArray;
    
            /**
             * Creates a new SplitflapCommand instance using the specified properties.
             * @function create
             * @memberof PB.SplitflapCommand
             * @static
             * @param {PB.ISplitflapCommand=} [properties] Properties to set
             * @returns {PB.SplitflapCommand} SplitflapCommand instance
             */
            SplitflapCommand.create = function create(properties) {
                return new SplitflapCommand(properties);
            };
    
            /**
             * Encodes the specified SplitflapCommand message. Does not implicitly {@link PB.SplitflapCommand.verify|verify} messages.
             * @function encode
             * @memberof PB.SplitflapCommand
             * @static
             * @param {PB.ISplitflapCommand} message SplitflapCommand message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            SplitflapCommand.encode = function encode(message, writer) {
                if (!writer)
                    writer = $Writer.create();
                if (message.modules != null && message.modules.length)
                    for (var i = 0; i < message.modules.length; ++i)
                        $root.PB.SplitflapCommand.ModuleCommand.encode(message.modules[i], writer.uint32(/* id 2, wireType 2 =*/18).fork()).ldelim();
                return writer;
            };
    
            /**
             * Encodes the specified SplitflapCommand message, length delimited. Does not implicitly {@link PB.SplitflapCommand.verify|verify} messages.
             * @function encodeDelimited
             * @memberof PB.SplitflapCommand
             * @static
             * @param {PB.ISplitflapCommand} message SplitflapCommand message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            SplitflapCommand.encodeDelimited = function encodeDelimited(message, writer) {
                return this.encode(message, writer).ldelim();
            };
    
            /**
             * Decodes a SplitflapCommand message from the specified reader or buffer.
             * @function decode
             * @memberof PB.SplitflapCommand
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @param {number} [length] Message length if known beforehand
             * @returns {PB.SplitflapCommand} SplitflapCommand
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            SplitflapCommand.decode = function decode(reader, length) {
                if (!(reader instanceof $Reader))
                    reader = $Reader.create(reader);
                var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.SplitflapCommand();
                while (reader.pos < end) {
                    var tag = reader.uint32();
                    switch (tag >>> 3) {
                    case 2:
                        if (!(message.modules && message.modules.length))
                            message.modules = [];
                        message.modules.push($root.PB.SplitflapCommand.ModuleCommand.decode(reader, reader.uint32()));
                        break;
                    default:
                        reader.skipType(tag & 7);
                        break;
                    }
                }
                return message;
            };
    
            /**
             * Decodes a SplitflapCommand message from the specified reader or buffer, length delimited.
             * @function decodeDelimited
             * @memberof PB.SplitflapCommand
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @returns {PB.SplitflapCommand} SplitflapCommand
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            SplitflapCommand.decodeDelimited = function decodeDelimited(reader) {
                if (!(reader instanceof $Reader))
                    reader = new $Reader(reader);
                return this.decode(reader, reader.uint32());
            };
    
            /**
             * Verifies a SplitflapCommand message.
             * @function verify
             * @memberof PB.SplitflapCommand
             * @static
             * @param {Object.<string,*>} message Plain object to verify
             * @returns {string|null} `null` if valid, otherwise the reason why it is not
             */
            SplitflapCommand.verify = function verify(message) {
                if (typeof message !== "object" || message === null)
                    return "object expected";
                if (message.modules != null && message.hasOwnProperty("modules")) {
                    if (!Array.isArray(message.modules))
                        return "modules: array expected";
                    for (var i = 0; i < message.modules.length; ++i) {
                        var error = $root.PB.SplitflapCommand.ModuleCommand.verify(message.modules[i]);
                        if (error)
                            return "modules." + error;
                    }
                }
                return null;
            };
    
            /**
             * Creates a SplitflapCommand message from a plain object. Also converts values to their respective internal types.
             * @function fromObject
             * @memberof PB.SplitflapCommand
             * @static
             * @param {Object.<string,*>} object Plain object
             * @returns {PB.SplitflapCommand} SplitflapCommand
             */
            SplitflapCommand.fromObject = function fromObject(object) {
                if (object instanceof $root.PB.SplitflapCommand)
                    return object;
                var message = new $root.PB.SplitflapCommand();
                if (object.modules) {
                    if (!Array.isArray(object.modules))
                        throw TypeError(".PB.SplitflapCommand.modules: array expected");
                    message.modules = [];
                    for (var i = 0; i < object.modules.length; ++i) {
                        if (typeof object.modules[i] !== "object")
                            throw TypeError(".PB.SplitflapCommand.modules: object expected");
                        message.modules[i] = $root.PB.SplitflapCommand.ModuleCommand.fromObject(object.modules[i]);
                    }
                }
                return message;
            };
    
            /**
             * Creates a plain object from a SplitflapCommand message. Also converts values to other types if specified.
             * @function toObject
             * @memberof PB.SplitflapCommand
             * @static
             * @param {PB.SplitflapCommand} message SplitflapCommand
             * @param {$protobuf.IConversionOptions} [options] Conversion options
             * @returns {Object.<string,*>} Plain object
             */
            SplitflapCommand.toObject = function toObject(message, options) {
                if (!options)
                    options = {};
                var object = {};
                if (options.arrays || options.defaults)
                    object.modules = [];
                if (message.modules && message.modules.length) {
                    object.modules = [];
                    for (var j = 0; j < message.modules.length; ++j)
                        object.modules[j] = $root.PB.SplitflapCommand.ModuleCommand.toObject(message.modules[j], options);
                }
                return object;
            };
    
            /**
             * Converts this SplitflapCommand to JSON.
             * @function toJSON
             * @memberof PB.SplitflapCommand
             * @instance
             * @returns {Object.<string,*>} JSON object
             */
            SplitflapCommand.prototype.toJSON = function toJSON() {
                return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
            };
    
            SplitflapCommand.ModuleCommand = (function() {
    
                /**
                 * Properties of a ModuleCommand.
                 * @memberof PB.SplitflapCommand
                 * @interface IModuleCommand
                 * @property {PB.SplitflapCommand.ModuleCommand.Action|null} [action] ModuleCommand action
                 * @property {number|null} [param] ModuleCommand param
                 */
    
                /**
                 * Constructs a new ModuleCommand.
                 * @memberof PB.SplitflapCommand
                 * @classdesc Represents a ModuleCommand.
                 * @implements IModuleCommand
                 * @constructor
                 * @param {PB.SplitflapCommand.IModuleCommand=} [properties] Properties to set
                 */
                function ModuleCommand(properties) {
                    if (properties)
                        for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                            if (properties[keys[i]] != null)
                                this[keys[i]] = properties[keys[i]];
                }
    
                /**
                 * ModuleCommand action.
                 * @member {PB.SplitflapCommand.ModuleCommand.Action} action
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @instance
                 */
                ModuleCommand.prototype.action = 0;
    
                /**
                 * ModuleCommand param.
                 * @member {number} param
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @instance
                 */
                ModuleCommand.prototype.param = 0;
    
                /**
                 * Creates a new ModuleCommand instance using the specified properties.
                 * @function create
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @static
                 * @param {PB.SplitflapCommand.IModuleCommand=} [properties] Properties to set
                 * @returns {PB.SplitflapCommand.ModuleCommand} ModuleCommand instance
                 */
                ModuleCommand.create = function create(properties) {
                    return new ModuleCommand(properties);
                };
    
                /**
                 * Encodes the specified ModuleCommand message. Does not implicitly {@link PB.SplitflapCommand.ModuleCommand.verify|verify} messages.
                 * @function encode
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @static
                 * @param {PB.SplitflapCommand.IModuleCommand} message ModuleCommand message or plain object to encode
                 * @param {$protobuf.Writer} [writer] Writer to encode to
                 * @returns {$protobuf.Writer} Writer
                 */
                ModuleCommand.encode = function encode(message, writer) {
                    if (!writer)
                        writer = $Writer.create();
                    if (message.action != null && Object.hasOwnProperty.call(message, "action"))
                        writer.uint32(/* id 1, wireType 0 =*/8).int32(message.action);
                    if (message.param != null && Object.hasOwnProperty.call(message, "param"))
                        writer.uint32(/* id 2, wireType 0 =*/16).uint32(message.param);
                    return writer;
                };
    
                /**
                 * Encodes the specified ModuleCommand message, length delimited. Does not implicitly {@link PB.SplitflapCommand.ModuleCommand.verify|verify} messages.
                 * @function encodeDelimited
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @static
                 * @param {PB.SplitflapCommand.IModuleCommand} message ModuleCommand message or plain object to encode
                 * @param {$protobuf.Writer} [writer] Writer to encode to
                 * @returns {$protobuf.Writer} Writer
                 */
                ModuleCommand.encodeDelimited = function encodeDelimited(message, writer) {
                    return this.encode(message, writer).ldelim();
                };
    
                /**
                 * Decodes a ModuleCommand message from the specified reader or buffer.
                 * @function decode
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @static
                 * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
                 * @param {number} [length] Message length if known beforehand
                 * @returns {PB.SplitflapCommand.ModuleCommand} ModuleCommand
                 * @throws {Error} If the payload is not a reader or valid buffer
                 * @throws {$protobuf.util.ProtocolError} If required fields are missing
                 */
                ModuleCommand.decode = function decode(reader, length) {
                    if (!(reader instanceof $Reader))
                        reader = $Reader.create(reader);
                    var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.SplitflapCommand.ModuleCommand();
                    while (reader.pos < end) {
                        var tag = reader.uint32();
                        switch (tag >>> 3) {
                        case 1:
                            message.action = reader.int32();
                            break;
                        case 2:
                            message.param = reader.uint32();
                            break;
                        default:
                            reader.skipType(tag & 7);
                            break;
                        }
                    }
                    return message;
                };
    
                /**
                 * Decodes a ModuleCommand message from the specified reader or buffer, length delimited.
                 * @function decodeDelimited
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @static
                 * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
                 * @returns {PB.SplitflapCommand.ModuleCommand} ModuleCommand
                 * @throws {Error} If the payload is not a reader or valid buffer
                 * @throws {$protobuf.util.ProtocolError} If required fields are missing
                 */
                ModuleCommand.decodeDelimited = function decodeDelimited(reader) {
                    if (!(reader instanceof $Reader))
                        reader = new $Reader(reader);
                    return this.decode(reader, reader.uint32());
                };
    
                /**
                 * Verifies a ModuleCommand message.
                 * @function verify
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @static
                 * @param {Object.<string,*>} message Plain object to verify
                 * @returns {string|null} `null` if valid, otherwise the reason why it is not
                 */
                ModuleCommand.verify = function verify(message) {
                    if (typeof message !== "object" || message === null)
                        return "object expected";
                    if (message.action != null && message.hasOwnProperty("action"))
                        switch (message.action) {
                        default:
                            return "action: enum value expected";
                        case 0:
                        case 1:
                        case 2:
                            break;
                        }
                    if (message.param != null && message.hasOwnProperty("param"))
                        if (!$util.isInteger(message.param))
                            return "param: integer expected";
                    return null;
                };
    
                /**
                 * Creates a ModuleCommand message from a plain object. Also converts values to their respective internal types.
                 * @function fromObject
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @static
                 * @param {Object.<string,*>} object Plain object
                 * @returns {PB.SplitflapCommand.ModuleCommand} ModuleCommand
                 */
                ModuleCommand.fromObject = function fromObject(object) {
                    if (object instanceof $root.PB.SplitflapCommand.ModuleCommand)
                        return object;
                    var message = new $root.PB.SplitflapCommand.ModuleCommand();
                    switch (object.action) {
                    case "NO_OP":
                    case 0:
                        message.action = 0;
                        break;
                    case "GO_TO_FLAP":
                    case 1:
                        message.action = 1;
                        break;
                    case "RESET_AND_HOME":
                    case 2:
                        message.action = 2;
                        break;
                    }
                    if (object.param != null)
                        message.param = object.param >>> 0;
                    return message;
                };
    
                /**
                 * Creates a plain object from a ModuleCommand message. Also converts values to other types if specified.
                 * @function toObject
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @static
                 * @param {PB.SplitflapCommand.ModuleCommand} message ModuleCommand
                 * @param {$protobuf.IConversionOptions} [options] Conversion options
                 * @returns {Object.<string,*>} Plain object
                 */
                ModuleCommand.toObject = function toObject(message, options) {
                    if (!options)
                        options = {};
                    var object = {};
                    if (options.defaults) {
                        object.action = options.enums === String ? "NO_OP" : 0;
                        object.param = 0;
                    }
                    if (message.action != null && message.hasOwnProperty("action"))
                        object.action = options.enums === String ? $root.PB.SplitflapCommand.ModuleCommand.Action[message.action] : message.action;
                    if (message.param != null && message.hasOwnProperty("param"))
                        object.param = message.param;
                    return object;
                };
    
                /**
                 * Converts this ModuleCommand to JSON.
                 * @function toJSON
                 * @memberof PB.SplitflapCommand.ModuleCommand
                 * @instance
                 * @returns {Object.<string,*>} JSON object
                 */
                ModuleCommand.prototype.toJSON = function toJSON() {
                    return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
                };
    
                /**
                 * Action enum.
                 * @name PB.SplitflapCommand.ModuleCommand.Action
                 * @enum {number}
                 * @property {number} NO_OP=0 NO_OP value
                 * @property {number} GO_TO_FLAP=1 GO_TO_FLAP value
                 * @property {number} RESET_AND_HOME=2 RESET_AND_HOME value
                 */
                ModuleCommand.Action = (function() {
                    var valuesById = {}, values = Object.create(valuesById);
                    values[valuesById[0] = "NO_OP"] = 0;
                    values[valuesById[1] = "GO_TO_FLAP"] = 1;
                    values[valuesById[2] = "RESET_AND_HOME"] = 2;
                    return values;
                })();
    
                return ModuleCommand;
            })();
    
            return SplitflapCommand;
        })();
    
        PB.SplitflapConfig = (function() {
    
            /**
             * Properties of a SplitflapConfig.
             * @memberof PB
             * @interface ISplitflapConfig
             * @property {Array.<PB.SplitflapConfig.IModuleConfig>|null} [modules] SplitflapConfig modules
             */
    
            /**
             * Constructs a new SplitflapConfig.
             * @memberof PB
             * @classdesc Represents a SplitflapConfig.
             * @implements ISplitflapConfig
             * @constructor
             * @param {PB.ISplitflapConfig=} [properties] Properties to set
             */
            function SplitflapConfig(properties) {
                this.modules = [];
                if (properties)
                    for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                        if (properties[keys[i]] != null)
                            this[keys[i]] = properties[keys[i]];
            }
    
            /**
             * SplitflapConfig modules.
             * @member {Array.<PB.SplitflapConfig.IModuleConfig>} modules
             * @memberof PB.SplitflapConfig
             * @instance
             */
            SplitflapConfig.prototype.modules = $util.emptyArray;
    
            /**
             * Creates a new SplitflapConfig instance using the specified properties.
             * @function create
             * @memberof PB.SplitflapConfig
             * @static
             * @param {PB.ISplitflapConfig=} [properties] Properties to set
             * @returns {PB.SplitflapConfig} SplitflapConfig instance
             */
            SplitflapConfig.create = function create(properties) {
                return new SplitflapConfig(properties);
            };
    
            /**
             * Encodes the specified SplitflapConfig message. Does not implicitly {@link PB.SplitflapConfig.verify|verify} messages.
             * @function encode
             * @memberof PB.SplitflapConfig
             * @static
             * @param {PB.ISplitflapConfig} message SplitflapConfig message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            SplitflapConfig.encode = function encode(message, writer) {
                if (!writer)
                    writer = $Writer.create();
                if (message.modules != null && message.modules.length)
                    for (var i = 0; i < message.modules.length; ++i)
                        $root.PB.SplitflapConfig.ModuleConfig.encode(message.modules[i], writer.uint32(/* id 1, wireType 2 =*/10).fork()).ldelim();
                return writer;
            };
    
            /**
             * Encodes the specified SplitflapConfig message, length delimited. Does not implicitly {@link PB.SplitflapConfig.verify|verify} messages.
             * @function encodeDelimited
             * @memberof PB.SplitflapConfig
             * @static
             * @param {PB.ISplitflapConfig} message SplitflapConfig message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            SplitflapConfig.encodeDelimited = function encodeDelimited(message, writer) {
                return this.encode(message, writer).ldelim();
            };
    
            /**
             * Decodes a SplitflapConfig message from the specified reader or buffer.
             * @function decode
             * @memberof PB.SplitflapConfig
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @param {number} [length] Message length if known beforehand
             * @returns {PB.SplitflapConfig} SplitflapConfig
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            SplitflapConfig.decode = function decode(reader, length) {
                if (!(reader instanceof $Reader))
                    reader = $Reader.create(reader);
                var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.SplitflapConfig();
                while (reader.pos < end) {
                    var tag = reader.uint32();
                    switch (tag >>> 3) {
                    case 1:
                        if (!(message.modules && message.modules.length))
                            message.modules = [];
                        message.modules.push($root.PB.SplitflapConfig.ModuleConfig.decode(reader, reader.uint32()));
                        break;
                    default:
                        reader.skipType(tag & 7);
                        break;
                    }
                }
                return message;
            };
    
            /**
             * Decodes a SplitflapConfig message from the specified reader or buffer, length delimited.
             * @function decodeDelimited
             * @memberof PB.SplitflapConfig
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @returns {PB.SplitflapConfig} SplitflapConfig
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            SplitflapConfig.decodeDelimited = function decodeDelimited(reader) {
                if (!(reader instanceof $Reader))
                    reader = new $Reader(reader);
                return this.decode(reader, reader.uint32());
            };
    
            /**
             * Verifies a SplitflapConfig message.
             * @function verify
             * @memberof PB.SplitflapConfig
             * @static
             * @param {Object.<string,*>} message Plain object to verify
             * @returns {string|null} `null` if valid, otherwise the reason why it is not
             */
            SplitflapConfig.verify = function verify(message) {
                if (typeof message !== "object" || message === null)
                    return "object expected";
                if (message.modules != null && message.hasOwnProperty("modules")) {
                    if (!Array.isArray(message.modules))
                        return "modules: array expected";
                    for (var i = 0; i < message.modules.length; ++i) {
                        var error = $root.PB.SplitflapConfig.ModuleConfig.verify(message.modules[i]);
                        if (error)
                            return "modules." + error;
                    }
                }
                return null;
            };
    
            /**
             * Creates a SplitflapConfig message from a plain object. Also converts values to their respective internal types.
             * @function fromObject
             * @memberof PB.SplitflapConfig
             * @static
             * @param {Object.<string,*>} object Plain object
             * @returns {PB.SplitflapConfig} SplitflapConfig
             */
            SplitflapConfig.fromObject = function fromObject(object) {
                if (object instanceof $root.PB.SplitflapConfig)
                    return object;
                var message = new $root.PB.SplitflapConfig();
                if (object.modules) {
                    if (!Array.isArray(object.modules))
                        throw TypeError(".PB.SplitflapConfig.modules: array expected");
                    message.modules = [];
                    for (var i = 0; i < object.modules.length; ++i) {
                        if (typeof object.modules[i] !== "object")
                            throw TypeError(".PB.SplitflapConfig.modules: object expected");
                        message.modules[i] = $root.PB.SplitflapConfig.ModuleConfig.fromObject(object.modules[i]);
                    }
                }
                return message;
            };
    
            /**
             * Creates a plain object from a SplitflapConfig message. Also converts values to other types if specified.
             * @function toObject
             * @memberof PB.SplitflapConfig
             * @static
             * @param {PB.SplitflapConfig} message SplitflapConfig
             * @param {$protobuf.IConversionOptions} [options] Conversion options
             * @returns {Object.<string,*>} Plain object
             */
            SplitflapConfig.toObject = function toObject(message, options) {
                if (!options)
                    options = {};
                var object = {};
                if (options.arrays || options.defaults)
                    object.modules = [];
                if (message.modules && message.modules.length) {
                    object.modules = [];
                    for (var j = 0; j < message.modules.length; ++j)
                        object.modules[j] = $root.PB.SplitflapConfig.ModuleConfig.toObject(message.modules[j], options);
                }
                return object;
            };
    
            /**
             * Converts this SplitflapConfig to JSON.
             * @function toJSON
             * @memberof PB.SplitflapConfig
             * @instance
             * @returns {Object.<string,*>} JSON object
             */
            SplitflapConfig.prototype.toJSON = function toJSON() {
                return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
            };
    
            SplitflapConfig.ModuleConfig = (function() {
    
                /**
                 * Properties of a ModuleConfig.
                 * @memberof PB.SplitflapConfig
                 * @interface IModuleConfig
                 * @property {number|null} [targetFlapIndex] ModuleConfig targetFlapIndex
                 * @property {number|null} [movementNonce] Value that triggers a movement upon change. If unused, only changes to target_flap_index
                 * will trigger a movement. This can be used to trigger a full revolution back to the *same*
                 * flap index.
                 * 
                 * NOTE: Must be < 256
                 * @property {number|null} [resetNonce] Value that triggers a reset (clear error counters, re-home) upon change. If unused,
                 * module will only re-home upon recoverable errors, and error counters will continue
                 * to increase until overflow.
                 * 
                 * NOTE: Must be < 256
                 */
    
                /**
                 * Constructs a new ModuleConfig.
                 * @memberof PB.SplitflapConfig
                 * @classdesc Represents a ModuleConfig.
                 * @implements IModuleConfig
                 * @constructor
                 * @param {PB.SplitflapConfig.IModuleConfig=} [properties] Properties to set
                 */
                function ModuleConfig(properties) {
                    if (properties)
                        for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                            if (properties[keys[i]] != null)
                                this[keys[i]] = properties[keys[i]];
                }
    
                /**
                 * ModuleConfig targetFlapIndex.
                 * @member {number} targetFlapIndex
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @instance
                 */
                ModuleConfig.prototype.targetFlapIndex = 0;
    
                /**
                 * Value that triggers a movement upon change. If unused, only changes to target_flap_index
                 * will trigger a movement. This can be used to trigger a full revolution back to the *same*
                 * flap index.
                 * 
                 * NOTE: Must be < 256
                 * @member {number} movementNonce
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @instance
                 */
                ModuleConfig.prototype.movementNonce = 0;
    
                /**
                 * Value that triggers a reset (clear error counters, re-home) upon change. If unused,
                 * module will only re-home upon recoverable errors, and error counters will continue
                 * to increase until overflow.
                 * 
                 * NOTE: Must be < 256
                 * @member {number} resetNonce
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @instance
                 */
                ModuleConfig.prototype.resetNonce = 0;
    
                /**
                 * Creates a new ModuleConfig instance using the specified properties.
                 * @function create
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @static
                 * @param {PB.SplitflapConfig.IModuleConfig=} [properties] Properties to set
                 * @returns {PB.SplitflapConfig.ModuleConfig} ModuleConfig instance
                 */
                ModuleConfig.create = function create(properties) {
                    return new ModuleConfig(properties);
                };
    
                /**
                 * Encodes the specified ModuleConfig message. Does not implicitly {@link PB.SplitflapConfig.ModuleConfig.verify|verify} messages.
                 * @function encode
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @static
                 * @param {PB.SplitflapConfig.IModuleConfig} message ModuleConfig message or plain object to encode
                 * @param {$protobuf.Writer} [writer] Writer to encode to
                 * @returns {$protobuf.Writer} Writer
                 */
                ModuleConfig.encode = function encode(message, writer) {
                    if (!writer)
                        writer = $Writer.create();
                    if (message.targetFlapIndex != null && Object.hasOwnProperty.call(message, "targetFlapIndex"))
                        writer.uint32(/* id 1, wireType 0 =*/8).uint32(message.targetFlapIndex);
                    if (message.movementNonce != null && Object.hasOwnProperty.call(message, "movementNonce"))
                        writer.uint32(/* id 2, wireType 0 =*/16).uint32(message.movementNonce);
                    if (message.resetNonce != null && Object.hasOwnProperty.call(message, "resetNonce"))
                        writer.uint32(/* id 3, wireType 0 =*/24).uint32(message.resetNonce);
                    return writer;
                };
    
                /**
                 * Encodes the specified ModuleConfig message, length delimited. Does not implicitly {@link PB.SplitflapConfig.ModuleConfig.verify|verify} messages.
                 * @function encodeDelimited
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @static
                 * @param {PB.SplitflapConfig.IModuleConfig} message ModuleConfig message or plain object to encode
                 * @param {$protobuf.Writer} [writer] Writer to encode to
                 * @returns {$protobuf.Writer} Writer
                 */
                ModuleConfig.encodeDelimited = function encodeDelimited(message, writer) {
                    return this.encode(message, writer).ldelim();
                };
    
                /**
                 * Decodes a ModuleConfig message from the specified reader or buffer.
                 * @function decode
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @static
                 * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
                 * @param {number} [length] Message length if known beforehand
                 * @returns {PB.SplitflapConfig.ModuleConfig} ModuleConfig
                 * @throws {Error} If the payload is not a reader or valid buffer
                 * @throws {$protobuf.util.ProtocolError} If required fields are missing
                 */
                ModuleConfig.decode = function decode(reader, length) {
                    if (!(reader instanceof $Reader))
                        reader = $Reader.create(reader);
                    var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.SplitflapConfig.ModuleConfig();
                    while (reader.pos < end) {
                        var tag = reader.uint32();
                        switch (tag >>> 3) {
                        case 1:
                            message.targetFlapIndex = reader.uint32();
                            break;
                        case 2:
                            message.movementNonce = reader.uint32();
                            break;
                        case 3:
                            message.resetNonce = reader.uint32();
                            break;
                        default:
                            reader.skipType(tag & 7);
                            break;
                        }
                    }
                    return message;
                };
    
                /**
                 * Decodes a ModuleConfig message from the specified reader or buffer, length delimited.
                 * @function decodeDelimited
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @static
                 * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
                 * @returns {PB.SplitflapConfig.ModuleConfig} ModuleConfig
                 * @throws {Error} If the payload is not a reader or valid buffer
                 * @throws {$protobuf.util.ProtocolError} If required fields are missing
                 */
                ModuleConfig.decodeDelimited = function decodeDelimited(reader) {
                    if (!(reader instanceof $Reader))
                        reader = new $Reader(reader);
                    return this.decode(reader, reader.uint32());
                };
    
                /**
                 * Verifies a ModuleConfig message.
                 * @function verify
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @static
                 * @param {Object.<string,*>} message Plain object to verify
                 * @returns {string|null} `null` if valid, otherwise the reason why it is not
                 */
                ModuleConfig.verify = function verify(message) {
                    if (typeof message !== "object" || message === null)
                        return "object expected";
                    if (message.targetFlapIndex != null && message.hasOwnProperty("targetFlapIndex"))
                        if (!$util.isInteger(message.targetFlapIndex))
                            return "targetFlapIndex: integer expected";
                    if (message.movementNonce != null && message.hasOwnProperty("movementNonce"))
                        if (!$util.isInteger(message.movementNonce))
                            return "movementNonce: integer expected";
                    if (message.resetNonce != null && message.hasOwnProperty("resetNonce"))
                        if (!$util.isInteger(message.resetNonce))
                            return "resetNonce: integer expected";
                    return null;
                };
    
                /**
                 * Creates a ModuleConfig message from a plain object. Also converts values to their respective internal types.
                 * @function fromObject
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @static
                 * @param {Object.<string,*>} object Plain object
                 * @returns {PB.SplitflapConfig.ModuleConfig} ModuleConfig
                 */
                ModuleConfig.fromObject = function fromObject(object) {
                    if (object instanceof $root.PB.SplitflapConfig.ModuleConfig)
                        return object;
                    var message = new $root.PB.SplitflapConfig.ModuleConfig();
                    if (object.targetFlapIndex != null)
                        message.targetFlapIndex = object.targetFlapIndex >>> 0;
                    if (object.movementNonce != null)
                        message.movementNonce = object.movementNonce >>> 0;
                    if (object.resetNonce != null)
                        message.resetNonce = object.resetNonce >>> 0;
                    return message;
                };
    
                /**
                 * Creates a plain object from a ModuleConfig message. Also converts values to other types if specified.
                 * @function toObject
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @static
                 * @param {PB.SplitflapConfig.ModuleConfig} message ModuleConfig
                 * @param {$protobuf.IConversionOptions} [options] Conversion options
                 * @returns {Object.<string,*>} Plain object
                 */
                ModuleConfig.toObject = function toObject(message, options) {
                    if (!options)
                        options = {};
                    var object = {};
                    if (options.defaults) {
                        object.targetFlapIndex = 0;
                        object.movementNonce = 0;
                        object.resetNonce = 0;
                    }
                    if (message.targetFlapIndex != null && message.hasOwnProperty("targetFlapIndex"))
                        object.targetFlapIndex = message.targetFlapIndex;
                    if (message.movementNonce != null && message.hasOwnProperty("movementNonce"))
                        object.movementNonce = message.movementNonce;
                    if (message.resetNonce != null && message.hasOwnProperty("resetNonce"))
                        object.resetNonce = message.resetNonce;
                    return object;
                };
    
                /**
                 * Converts this ModuleConfig to JSON.
                 * @function toJSON
                 * @memberof PB.SplitflapConfig.ModuleConfig
                 * @instance
                 * @returns {Object.<string,*>} JSON object
                 */
                ModuleConfig.prototype.toJSON = function toJSON() {
                    return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
                };
    
                return ModuleConfig;
            })();
    
            return SplitflapConfig;
        })();
    
        PB.ToSplitflap = (function() {
    
            /**
             * Properties of a ToSplitflap.
             * @memberof PB
             * @interface IToSplitflap
             * @property {number|null} [nonce] ToSplitflap nonce
             * @property {PB.ISplitflapCommand|null} [splitflapCommand] ToSplitflap splitflapCommand
             * @property {PB.ISplitflapConfig|null} [splitflapConfig] ToSplitflap splitflapConfig
             */
    
            /**
             * Constructs a new ToSplitflap.
             * @memberof PB
             * @classdesc Represents a ToSplitflap.
             * @implements IToSplitflap
             * @constructor
             * @param {PB.IToSplitflap=} [properties] Properties to set
             */
            function ToSplitflap(properties) {
                if (properties)
                    for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                        if (properties[keys[i]] != null)
                            this[keys[i]] = properties[keys[i]];
            }
    
            /**
             * ToSplitflap nonce.
             * @member {number} nonce
             * @memberof PB.ToSplitflap
             * @instance
             */
            ToSplitflap.prototype.nonce = 0;
    
            /**
             * ToSplitflap splitflapCommand.
             * @member {PB.ISplitflapCommand|null|undefined} splitflapCommand
             * @memberof PB.ToSplitflap
             * @instance
             */
            ToSplitflap.prototype.splitflapCommand = null;
    
            /**
             * ToSplitflap splitflapConfig.
             * @member {PB.ISplitflapConfig|null|undefined} splitflapConfig
             * @memberof PB.ToSplitflap
             * @instance
             */
            ToSplitflap.prototype.splitflapConfig = null;
    
            // OneOf field names bound to virtual getters and setters
            var $oneOfFields;
    
            /**
             * ToSplitflap payload.
             * @member {"splitflapCommand"|"splitflapConfig"|undefined} payload
             * @memberof PB.ToSplitflap
             * @instance
             */
            Object.defineProperty(ToSplitflap.prototype, "payload", {
                get: $util.oneOfGetter($oneOfFields = ["splitflapCommand", "splitflapConfig"]),
                set: $util.oneOfSetter($oneOfFields)
            });
    
            /**
             * Creates a new ToSplitflap instance using the specified properties.
             * @function create
             * @memberof PB.ToSplitflap
             * @static
             * @param {PB.IToSplitflap=} [properties] Properties to set
             * @returns {PB.ToSplitflap} ToSplitflap instance
             */
            ToSplitflap.create = function create(properties) {
                return new ToSplitflap(properties);
            };
    
            /**
             * Encodes the specified ToSplitflap message. Does not implicitly {@link PB.ToSplitflap.verify|verify} messages.
             * @function encode
             * @memberof PB.ToSplitflap
             * @static
             * @param {PB.IToSplitflap} message ToSplitflap message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            ToSplitflap.encode = function encode(message, writer) {
                if (!writer)
                    writer = $Writer.create();
                if (message.nonce != null && Object.hasOwnProperty.call(message, "nonce"))
                    writer.uint32(/* id 1, wireType 0 =*/8).uint32(message.nonce);
                if (message.splitflapCommand != null && Object.hasOwnProperty.call(message, "splitflapCommand"))
                    $root.PB.SplitflapCommand.encode(message.splitflapCommand, writer.uint32(/* id 2, wireType 2 =*/18).fork()).ldelim();
                if (message.splitflapConfig != null && Object.hasOwnProperty.call(message, "splitflapConfig"))
                    $root.PB.SplitflapConfig.encode(message.splitflapConfig, writer.uint32(/* id 3, wireType 2 =*/26).fork()).ldelim();
                return writer;
            };
    
            /**
             * Encodes the specified ToSplitflap message, length delimited. Does not implicitly {@link PB.ToSplitflap.verify|verify} messages.
             * @function encodeDelimited
             * @memberof PB.ToSplitflap
             * @static
             * @param {PB.IToSplitflap} message ToSplitflap message or plain object to encode
             * @param {$protobuf.Writer} [writer] Writer to encode to
             * @returns {$protobuf.Writer} Writer
             */
            ToSplitflap.encodeDelimited = function encodeDelimited(message, writer) {
                return this.encode(message, writer).ldelim();
            };
    
            /**
             * Decodes a ToSplitflap message from the specified reader or buffer.
             * @function decode
             * @memberof PB.ToSplitflap
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @param {number} [length] Message length if known beforehand
             * @returns {PB.ToSplitflap} ToSplitflap
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            ToSplitflap.decode = function decode(reader, length) {
                if (!(reader instanceof $Reader))
                    reader = $Reader.create(reader);
                var end = length === undefined ? reader.len : reader.pos + length, message = new $root.PB.ToSplitflap();
                while (reader.pos < end) {
                    var tag = reader.uint32();
                    switch (tag >>> 3) {
                    case 1:
                        message.nonce = reader.uint32();
                        break;
                    case 2:
                        message.splitflapCommand = $root.PB.SplitflapCommand.decode(reader, reader.uint32());
                        break;
                    case 3:
                        message.splitflapConfig = $root.PB.SplitflapConfig.decode(reader, reader.uint32());
                        break;
                    default:
                        reader.skipType(tag & 7);
                        break;
                    }
                }
                return message;
            };
    
            /**
             * Decodes a ToSplitflap message from the specified reader or buffer, length delimited.
             * @function decodeDelimited
             * @memberof PB.ToSplitflap
             * @static
             * @param {$protobuf.Reader|Uint8Array} reader Reader or buffer to decode from
             * @returns {PB.ToSplitflap} ToSplitflap
             * @throws {Error} If the payload is not a reader or valid buffer
             * @throws {$protobuf.util.ProtocolError} If required fields are missing
             */
            ToSplitflap.decodeDelimited = function decodeDelimited(reader) {
                if (!(reader instanceof $Reader))
                    reader = new $Reader(reader);
                return this.decode(reader, reader.uint32());
            };
    
            /**
             * Verifies a ToSplitflap message.
             * @function verify
             * @memberof PB.ToSplitflap
             * @static
             * @param {Object.<string,*>} message Plain object to verify
             * @returns {string|null} `null` if valid, otherwise the reason why it is not
             */
            ToSplitflap.verify = function verify(message) {
                if (typeof message !== "object" || message === null)
                    return "object expected";
                var properties = {};
                if (message.nonce != null && message.hasOwnProperty("nonce"))
                    if (!$util.isInteger(message.nonce))
                        return "nonce: integer expected";
                if (message.splitflapCommand != null && message.hasOwnProperty("splitflapCommand")) {
                    properties.payload = 1;
                    {
                        var error = $root.PB.SplitflapCommand.verify(message.splitflapCommand);
                        if (error)
                            return "splitflapCommand." + error;
                    }
                }
                if (message.splitflapConfig != null && message.hasOwnProperty("splitflapConfig")) {
                    if (properties.payload === 1)
                        return "payload: multiple values";
                    properties.payload = 1;
                    {
                        var error = $root.PB.SplitflapConfig.verify(message.splitflapConfig);
                        if (error)
                            return "splitflapConfig." + error;
                    }
                }
                return null;
            };
    
            /**
             * Creates a ToSplitflap message from a plain object. Also converts values to their respective internal types.
             * @function fromObject
             * @memberof PB.ToSplitflap
             * @static
             * @param {Object.<string,*>} object Plain object
             * @returns {PB.ToSplitflap} ToSplitflap
             */
            ToSplitflap.fromObject = function fromObject(object) {
                if (object instanceof $root.PB.ToSplitflap)
                    return object;
                var message = new $root.PB.ToSplitflap();
                if (object.nonce != null)
                    message.nonce = object.nonce >>> 0;
                if (object.splitflapCommand != null) {
                    if (typeof object.splitflapCommand !== "object")
                        throw TypeError(".PB.ToSplitflap.splitflapCommand: object expected");
                    message.splitflapCommand = $root.PB.SplitflapCommand.fromObject(object.splitflapCommand);
                }
                if (object.splitflapConfig != null) {
                    if (typeof object.splitflapConfig !== "object")
                        throw TypeError(".PB.ToSplitflap.splitflapConfig: object expected");
                    message.splitflapConfig = $root.PB.SplitflapConfig.fromObject(object.splitflapConfig);
                }
                return message;
            };
    
            /**
             * Creates a plain object from a ToSplitflap message. Also converts values to other types if specified.
             * @function toObject
             * @memberof PB.ToSplitflap
             * @static
             * @param {PB.ToSplitflap} message ToSplitflap
             * @param {$protobuf.IConversionOptions} [options] Conversion options
             * @returns {Object.<string,*>} Plain object
             */
            ToSplitflap.toObject = function toObject(message, options) {
                if (!options)
                    options = {};
                var object = {};
                if (options.defaults)
                    object.nonce = 0;
                if (message.nonce != null && message.hasOwnProperty("nonce"))
                    object.nonce = message.nonce;
                if (message.splitflapCommand != null && message.hasOwnProperty("splitflapCommand")) {
                    object.splitflapCommand = $root.PB.SplitflapCommand.toObject(message.splitflapCommand, options);
                    if (options.oneofs)
                        object.payload = "splitflapCommand";
                }
                if (message.splitflapConfig != null && message.hasOwnProperty("splitflapConfig")) {
                    object.splitflapConfig = $root.PB.SplitflapConfig.toObject(message.splitflapConfig, options);
                    if (options.oneofs)
                        object.payload = "splitflapConfig";
                }
                return object;
            };
    
            /**
             * Converts this ToSplitflap to JSON.
             * @function toJSON
             * @memberof PB.ToSplitflap
             * @instance
             * @returns {Object.<string,*>} JSON object
             */
            ToSplitflap.prototype.toJSON = function toJSON() {
                return this.constructor.toObject(this, $protobuf.util.toJSONOptions);
            };
    
            return ToSplitflap;
        })();
    
        return PB;
    })();

    return $root;
});
