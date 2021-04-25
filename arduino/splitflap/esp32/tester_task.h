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

#include <Arduino.h>

#include "splitflap_task.h"
#include "task.h"


class TesterTask : public Task<TesterTask> {
    friend class Task<TesterTask>; // Allow base Task to invoke protected run()

    public:
        TesterTask(SplitflapTask& splitflapTask, const uint8_t taskCore);

    protected:
        void run();

    private:
        SplitflapTask& splitflap_task_;
};
