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
#pragma once

#include "serial_protocol.h"
#include "../proto_gen/splitflap.pb.h"

class SerialProtoProtocol : public SerialProtocol {
    public:
        SerialProtoProtocol(SplitflapTask& splitflap_task) : SerialProtocol(splitflap_task) {}
        ~SerialProtoProtocol(){}
        void handleState(const SplitflapState& old_state, const SplitflapState& new_state);
        void handleRx(int byte);
        void log(String msg);
        void loop();

        void init();
    
    private:
        PB_SplitflapState pb_state_buffer_;
        uint8_t state_buffer_[PB_SplitflapState_size];
};
