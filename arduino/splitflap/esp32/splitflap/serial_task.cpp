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

SerialTask::SerialTask(SplitflapTask& splitflap_task, const uint8_t task_core) : 
        Task("Serial", 4096, 1, task_core), 
        splitflap_task_(splitflap_task),
        legacy_protocol_(splitflap_task),
        proto_protocol_(splitflap_task) {
}

void SerialTask::run() {
    // // Start in legacy protocol mode
    // SerialProtocol* current_protocol = &legacy_protocol;
    // legacy_protocol.init();

    // FIXME
    SerialProtocol* current_protocol = &proto_protocol_;


    SplitflapState last_state = {};
    while(1) {
        // TODO: set up task notifications or message queues for state changes instead of polling?
        SplitflapState new_state = splitflap_task_.getState();

        int state_compare = memcmp(&new_state, &last_state, sizeof(new_state));
        if (state_compare != 0) {
            current_protocol->handleState(last_state, new_state);
        }
        last_state = new_state;

        while (Serial.available() > 0) {
            int b = Serial.read();
            current_protocol->handleRx(b);

            // TODO: add mechanism for changing protocols...
        }
        delay(5);
    }
}

