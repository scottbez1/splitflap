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
    
        PB.FromSplitflap = (function() {
    
            /**
             * Properties of a FromSplitflap.
             * @memberof PB
             * @interface IFromSplitflap
             * @property {PB.ISplitflapState|null} [splitflapState] FromSplitflap splitflapState
             * @property {PB.ILog|null} [log] FromSplitflap log
             * @property {PB.IAck|null} [ack] FromSplitflap ack
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
    
            // OneOf field names bound to virtual getters and setters
            var $oneOfFields;
    
            /**
             * FromSplitflap payload.
             * @member {"splitflapState"|"log"|"ack"|undefined} payload
             * @memberof PB.FromSplitflap
             * @instance
             */
            Object.defineProperty(FromSplitflap.prototype, "payload", {
                get: $util.oneOfGetter($oneOfFields = ["splitflapState", "log", "ack"]),
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
    
        PB.ToSplitflap = (function() {
    
            /**
             * Properties of a ToSplitflap.
             * @memberof PB
             * @interface IToSplitflap
             * @property {number|null} [nonce] ToSplitflap nonce
             * @property {PB.ISplitflapCommand|null} [splitflapCommand] ToSplitflap splitflapCommand
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
    
            // OneOf field names bound to virtual getters and setters
            var $oneOfFields;
    
            /**
             * ToSplitflap payload.
             * @member {"splitflapCommand"|undefined} payload
             * @memberof PB.ToSplitflap
             * @instance
             */
            Object.defineProperty(ToSplitflap.prototype, "payload", {
                get: $util.oneOfGetter($oneOfFields = ["splitflapCommand"]),
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
