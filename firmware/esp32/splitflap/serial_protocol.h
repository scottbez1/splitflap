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

#include <functional>

#include "../core/logger.h"
#include "../core/splitflap_task.h"
#include "../proto_gen/splitflap.pb.h"

#define SERIAL_PROTOCOL_LEGACY 0
#define SERIAL_PROTOCOL_PROTO 1

typedef std::function<void(uint8_t)> ProtocolChangeCallback;
class SerialProtocol : public Logger {
    public:
        SerialProtocol(SplitflapTask& splitflap_task) : Logger(), splitflap_task_(splitflap_task) {}
        virtual ~SerialProtocol(){}

        virtual void loop() = 0;

        virtual void handleState(const SplitflapState& old_state, const SplitflapState& new_state) = 0;
        virtual void sendSupervisorState(PB_SupervisorState& supervisor_state) = 0;

        virtual void setProtocolChangeCallback(ProtocolChangeCallback cb) {
            protocol_change_callback_ = cb;
        }
    
    protected:
        SplitflapTask& splitflap_task_;
        ProtocolChangeCallback protocol_change_callback_;
};
