/*
   Copyright 2020 Scott Bezek and the splitflap contributors

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
#include <TFT_eSPI.h>

#include "../core/splitflap_task.h"
#include "../core/task.h"

class DisplayTask : public Task<DisplayTask> {
    friend class Task<DisplayTask>; // Allow base Task to invoke protected run()

    public:
        DisplayTask(SplitflapTask& splitflapTask, const uint8_t taskCore);
        ~DisplayTask();

        void setMessage(uint8_t i, String message);

    protected:
        void run();

    private:
        SplitflapTask& splitflap_task_;
        const SemaphoreHandle_t semaphore_;

        TFT_eSPI tft_ = TFT_eSPI();

        String messages_[2] = {};
};
