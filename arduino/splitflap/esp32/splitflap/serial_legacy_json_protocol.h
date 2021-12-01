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

class SerialLegacyJsonProtocol : public SerialProtocol {
    public:
        SerialLegacyJsonProtocol(SplitflapTask& splitflap_task, Stream& stream) : SerialProtocol(splitflap_task), stream_(stream) {}
        ~SerialLegacyJsonProtocol(){}
        void log(const char* msg) override;
        void loop() override;
        void handleState(const SplitflapState& old_state, const SplitflapState& new_state) override;
        void sendSupervisorState(PB_SupervisorState& supervisor_state) override;

        void init();
    
    private:
        Stream& stream_;
        SplitflapState latest_state_ = {};
        uint8_t recv_count_ = 0;
        char recv_buffer_[NUM_MODULES] = {};
        bool pending_move_response_ = false;
        uint32_t last_sensor_print_millis_ = 0;

        void dumpStatus(const SplitflapState& state);
};
