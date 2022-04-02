/*
   Copyright 2021 Scott Bezek and the splitflap contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "json11.hpp"

#include "serial_legacy_json_protocol.h"
#include "../proto_gen/splitflap.pb.h"

using namespace json11;

void SerialLegacyJsonProtocol::handleState(const SplitflapState& old_state, const SplitflapState& new_state) {
    bool all_stopped = true;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        all_stopped &= !new_state.modules[i].moving;
    }
    if (pending_move_response_ && all_stopped) {
        pending_move_response_ = false;
        dumpStatus(new_state);
    }

    latest_state_ = new_state;
}

void SerialLegacyJsonProtocol::log(const char* msg) {
    Json body = Json::object {
            {"type", "log"},
            {"msg", std::string(msg)},
    };
    stream_.println(body.dump().c_str());
}

void SerialLegacyJsonProtocol::loop() {
    if (latest_state_.mode == SplitflapMode::MODE_SENSOR_TEST) {
        if (millis() - last_sensor_print_millis_ > 200) {
            last_sensor_print_millis_ = millis();
            for (uint8_t i = 0; i < NUM_MODULES; i++) {
                stream_.write(latest_state_.modules[i].home_state ? '1' : '0');
            }
            stream_.println();
        }
    }

    while (stream_.available() > 0) {
        int b = stream_.read();
        if (b == 0) {
            if (protocol_change_callback_) {
                protocol_change_callback_(SERIAL_PROTOCOL_PROTO);
            }
            break;
        }
        if (b == '%') {
            bool new_sensor_test_state = latest_state_.mode != SplitflapMode::MODE_SENSOR_TEST;
            splitflap_task_.setSensorTest(new_sensor_test_state);
            stream_.print("{\"type\":\"sensor_test\", \"enabled\":");
            stream_.print(new_sensor_test_state ? "true" : "false");
            stream_.print("}\n");
        } else if (latest_state_.mode == SplitflapMode::MODE_RUN) {
            switch (b) {
                case '@':
                    splitflap_task_.resetAll();
                    break;
                case '#':
                    stream_.print("{\"type\":\"no_op\"}\n");
                    stream_.flush();
                    break;
                case '=':
                    recv_count_ = 0;
                    break;
                case '\n':
                    pending_move_response_ = true;
                    stream_.printf("{\"type\":\"move_echo\", \"dest\":\"");
                    stream_.flush();
                    for (uint8_t i = 0; i < recv_count_; i++) {
                        stream_.write(recv_buffer_[i]);
                    }
                    stream_.printf("\"}\n");
                    stream_.flush();
                    splitflap_task_.showString(recv_buffer_, recv_count_);
                    break;
                case '+':
                    if (recv_count_ == 1) {
                        for (uint8_t i = 1; i < NUM_MODULES; i++) {
                            recv_buffer_[i] = recv_buffer_[0];
                        }
                        splitflap_task_.showString(recv_buffer_, NUM_MODULES);
                    }
                    break;
                case '\r':
                    // Ignore
                    break;
                default:
                    if (recv_count_ > NUM_MODULES - 1) {
                        break;
                    }
                    recv_buffer_[recv_count_] = b;
                    recv_count_++;
                    break;
            }
        }
    }
}

void SerialLegacyJsonProtocol::sendSupervisorState(PB_SupervisorState& supervisor_state) {
    // Intentionally not implemented.
    // Advanced features like supervisor state are not supported via the legacy protocol; use
    // the proto protocol instead.
}

void SerialLegacyJsonProtocol::init() {
    stream_.print("\n\n\n");
    stream_.print("{\"type\":\"init\", \"num_modules\":");
    stream_.print(NUM_MODULES);
    stream_.print("}\n");
}

void SerialLegacyJsonProtocol::dumpStatus(const SplitflapState& state) {
    stream_.print("{\"type\":\"status\", \"modules\":[");
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        stream_.print("{\"state\":\"");
        switch (state.modules[i].state) {
            case NORMAL:
                stream_.print("normal");
                break;
            case LOOK_FOR_HOME:
                stream_.print("look_for_home");
                break;
            case SENSOR_ERROR:
                stream_.print("sensor_error");
                break;
            case PANIC:
                stream_.print("panic");
                break;
            case STATE_DISABLED:
                stream_.print("disabled");
                break;
        }
        stream_.print("\", \"flap\":\"");
        stream_.write(flaps[state.modules[i].flap_index]);
        stream_.print("\", \"count_missed_home\":");
        stream_.print(state.modules[i].count_missed_home);
        stream_.print(", \"count_unexpected_home\":");
        stream_.print(state.modules[i].count_unexpected_home);
        stream_.print("}");
        if (i < NUM_MODULES - 1) {
            stream_.print(", ");
        }
    }
    stream_.print("]}\n");
    stream_.flush();
}
