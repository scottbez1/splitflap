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

#include "../core/uart_stream.h"

SerialTask::SerialTask(SplitflapTask& splitflap_task, const uint8_t task_core) :
        Task("Serial", 16000, 1, task_core),
        Logger(),
        splitflap_task_(splitflap_task),
        stream_(),
        legacy_protocol_(splitflap_task_, stream_),
        proto_protocol_(splitflap_task_, stream_) {
    log_queue_ = xQueueCreate(10, sizeof(std::string *));
    assert(log_queue_ != NULL);

    supervisor_state_queue_ = xQueueCreate(1, sizeof(PB_SupervisorState));
    assert(supervisor_state_queue_ != NULL);
}

void SerialTask::run() {
    stream_.begin();

    // Start in legacy protocol mode
    legacy_protocol_.init();
    SerialProtocol* current_protocol = &legacy_protocol_;

    ProtocolChangeCallback protocol_change_callback = [this, &current_protocol] (uint8_t protocol) {
        switch (protocol) {
            case SERIAL_PROTOCOL_LEGACY:
                current_protocol = &legacy_protocol_;
                break;
            case SERIAL_PROTOCOL_PROTO:
                current_protocol = &proto_protocol_;
                break;
            default:
                log("Unknown protocol requested");
                break;
        }
    };

    legacy_protocol_.setProtocolChangeCallback(protocol_change_callback);
    proto_protocol_.setProtocolChangeCallback(protocol_change_callback);

    splitflap_task_.setLogger(this);

    SplitflapState last_state = {};
    while(1) {
        SplitflapState new_state = splitflap_task_.getState();

        if (new_state != last_state) {
            current_protocol->handleState(last_state, new_state);
            last_state = new_state;
        }

        current_protocol->loop();

        std::string* log_string;
        while (xQueueReceive(log_queue_, &log_string, 0) == pdTRUE) {
            current_protocol->log(log_string->c_str());
            delete log_string;
        }

        PB_SupervisorState supervisor_state;
        if (xQueueReceive(supervisor_state_queue_, &supervisor_state, 0) == pdTRUE) {
            current_protocol->sendSupervisorState(supervisor_state);
        }
        delay(1);
    }
}

void SerialTask::log(const char* msg) {
    // Allocate a string for the duration it's in the queue; it is free'd by the queue consumer
    std::string* msg_str = new std::string(msg);

    // Put string in queue (or drop if full to avoid blocking)
    xQueueSendToBack(log_queue_, &msg_str, 0);
}

void SerialTask::sendSupervisorState(PB_SupervisorState& supervisor_state) {
    // Only queue the latest supervisor state
    xQueueOverwrite(supervisor_state_queue_, &supervisor_state);
}