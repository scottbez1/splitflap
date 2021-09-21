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
#include "serial_task.h"

SerialTask::SerialTask(SplitflapTask& splitflap_task, const uint8_t task_core) : Task{"Serial", 4098, 1, task_core}, splitflap_task_{splitflap_task} {}

void SerialTask::run() {

    char recv_buffer[NUM_MODULES] = {};
    uint8_t recv_count = 0;
    bool pending_move_response = false;
    uint32_t last_sensor_print_millis = 0;

    Serial.print("\n\n\n");
    Serial.print("{\"type\":\"init\", \"num_modules\":");
    Serial.print(NUM_MODULES);
    Serial.print("}\n");

    while(1) {
        // TODO: set up task notifications or message queues for state changes instead of polling?
        SplitflapState state = splitflap_task_.getState();

        bool all_stopped = true;
        for (uint8_t i = 0; i < NUM_MODULES; i++) {
            all_stopped &= !state.modules[i].moving;
        }
        if (pending_move_response && all_stopped) {
            pending_move_response = false;
            dumpStatus(state);
        }

        if (state.mode == SplitflapMode::MODE_SENSOR_TEST) {
            if (millis() - last_sensor_print_millis > 200) {
                last_sensor_print_millis = millis();
                for (uint8_t i = 0; i < NUM_MODULES; i++) {
                    Serial.write(state.modules[i].home_state ? '1' : '0');
                }
                Serial.println();
            }
        }

        if (Serial.available() > 0) {
            int b = Serial.read();
            if (b == '%') {
                bool new_sensor_test_state = state.mode != SplitflapMode::MODE_SENSOR_TEST;
                splitflap_task_.setSensorTest(new_sensor_test_state);
                Serial.print("{\"type\":\"sensor_test\", \"enabled\":");
                Serial.print(new_sensor_test_state ? "true" : "false");
                Serial.print("}\n");
            } else if (state.mode == SplitflapMode::MODE_RUN) {
                switch (b) {
                    case '@':
                        splitflap_task_.resetAll();
                        break;
                    case '#':
                        Serial.print("{\"type\":\"no_op\"}\n");
                        Serial.flush();
                        break;
                    case '=':
                        recv_count = 0;
                        break;
                    case '\n':
                        pending_move_response = true;
                        Serial.print("{\"type\":\"move_echo\", \"dest\":\"");
                        Serial.flush();
                        for (uint8_t i = 0; i < recv_count; i++) {
                            Serial.write(recv_buffer[i]);
                        }
                        Serial.print("\"}\n");
                        Serial.flush();
                        splitflap_task_.showString(recv_buffer, recv_count);
                        break;
                    case '+':
                        if (recv_count == 1) {
                            for (uint8_t i = 1; i < NUM_MODULES; i++) {
                                recv_buffer[i] = recv_buffer[0];
                            }
                            splitflap_task_.showString(recv_buffer, NUM_MODULES);
                        }
                        break;
                    default:
                        if (recv_count > NUM_MODULES - 1) {
                            break;
                        }
                        recv_buffer[recv_count] = b;
                        recv_count++;
                        break;
                }
            }
        }
        delay(10);
    }
}

void SerialTask::dumpStatus(SplitflapState& state) {
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
