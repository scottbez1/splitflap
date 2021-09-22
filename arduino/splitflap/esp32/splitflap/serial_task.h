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

#include "config.h"

#include "../core/splitflap_task.h"
#include "../core/task.h"

#include "serial_legacy_json_protocol.h"
#include "serial_proto_protocol.h"

class SerialTask : public Task<SerialTask> {
    friend class Task<SerialTask>; // Allow base Task to invoke protected run()

    public:
        SerialTask(SplitflapTask& splitflapTask, const uint8_t task_core);

    protected:
        void run();

    private:
        SplitflapTask& splitflap_task_;

        SerialLegacyJsonProtocol legacy_protocol_;
        SerialProtoProtocol proto_protocol_;

        void dumpStatus(SplitflapState& state);
};