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
    Serial.println(body.dump().c_str());
}

void SerialLegacyJsonProtocol::loop() {
    if (latest_state_.mode == SplitflapMode::MODE_SENSOR_TEST) {
        if (millis() - last_sensor_print_millis_ > 200) {
            last_sensor_print_millis_ = millis();
            for (uint8_t i = 0; i < NUM_MODULES; i++) {
                Serial.write(latest_state_.modules[i].home_state ? '1' : '0');
            }
            Serial.println();
        }
    }

    while (Serial.available() > 0) {
        int b = Serial.read();
        if (b == '%') {
            bool new_sensor_test_state = latest_state_.mode != SplitflapMode::MODE_SENSOR_TEST;
            splitflap_task_.setSensorTest(new_sensor_test_state);
            Serial.print("{\"type\":\"sensor_test\", \"enabled\":");
            Serial.print(new_sensor_test_state ? "true" : "false");
            Serial.print("}\n");
        } else if (latest_state_.mode == SplitflapMode::MODE_RUN) {
            switch (b) {
                case '@':
                    splitflap_task_.resetAll();
                    break;
                case '#':
                    Serial.print("{\"type\":\"no_op\"}\n");
                    Serial.flush();
                    break;
                case '=':
                    recv_count_ = 0;
                    break;
                case '\n':
                    pending_move_response_ = true;
                    Serial.printf("{\"type\":\"move_echo\", \"dest\":\"");
                    Serial.flush();
                    for (uint8_t i = 0; i < recv_count_; i++) {
                        Serial.write(recv_buffer_[i]);
                    }
                    Serial.printf("\"}\n");
                    Serial.flush();
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

void SerialLegacyJsonProtocol::init() {
    Serial.print("\n\n\n");
    Serial.print("{\"type\":\"init\", \"num_modules\":");
    Serial.print(NUM_MODULES);
    Serial.print("}\n");
}

void SerialLegacyJsonProtocol::dumpStatus(const SplitflapState& state) {
    Serial.print("{\"type\":\"status\", \"modules\":[");
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        Serial.print("{\"state\":\"");
        switch (state.modules[i].state) {
            case NORMAL:
                Serial.print("normal");
                break;
            case LOOK_FOR_HOME:
                Serial.print("look_for_home");
                break;
            case SENSOR_ERROR:
                Serial.print("sensor_error");
                break;
            case PANIC:
                Serial.print("panic");
                break;
            case STATE_DISABLED:
                Serial.print("disabled");
                break;
        }
        Serial.print("\", \"flap\":\"");
        Serial.write(flaps[state.modules[i].flap_index]);
        Serial.print("\", \"count_missed_home\":");
        Serial.print(state.modules[i].count_missed_home);
        Serial.print(", \"count_unexpected_home\":");
        Serial.print(state.modules[i].count_unexpected_home);
        Serial.print("}");
        if (i < NUM_MODULES - 1) {
            Serial.print(", ");
        }
    }
    Serial.print("]}\n");
    Serial.flush();
}
