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
#include "../core/common.h"

#include "../proto_gen/splitflap.pb.h"

#include "pb_encode.h"
#include "serial_proto_protocol.h"


void SerialProtoProtocol::handleState(const SplitflapState& old_state, const SplitflapState& new_state) {
    pb_state_buffer_ = {};

    pb_state_buffer_.modules_count = NUM_MODULES;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        pb_state_buffer_.modules[i] = {
            .state = (PB_SplitflapState_ModuleState_State) new_state.modules[i].state,
            .flap_index = new_state.modules[i].flap_index,
            .moving = new_state.modules[i].moving,
            .home_state = new_state.modules[i].home_state,
            .count_unexpected_home = new_state.modules[i].count_unexpected_home,
            .count_missed_home = new_state.modules[i].count_missed_home,
        };
    }

    pb_ostream_t stream = pb_ostream_from_buffer(state_buffer_, COUNT_OF(state_buffer_));
    if (!pb_encode(&stream, PB_SplitflapState_fields, &pb_state_buffer_)) {
        Serial.println(stream.errmsg);
        Serial.flush();
        assert(false);
    }

    // TODO: CRC and Packet
    Serial.write(state_buffer_, stream.bytes_written);
}

void SerialProtoProtocol::handleRx(int b) {
}

void SerialProtoProtocol::log(String msg) {
}

void SerialProtoProtocol::loop() {
}
