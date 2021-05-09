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

#include "config.h"
#include "src/splitflap_module_data.h"

#include "task.h"

struct SplitflapModuleState {
    State state;
    uint8_t flapIndex;
    bool moving;
};

struct SplitflapState {
    SplitflapModuleState modules[NUM_MODULES];
};

class SplitflapTask : public Task<SplitflapTask> {
    friend class Task<SplitflapTask>; // Allow base Task to invoke protected run()

    public:
        SplitflapTask(const uint8_t taskCore);
        ~SplitflapTask();
        
        SplitflapState getState();

        void showString(const char *str, uint8_t length);
        void resetAll();
        bool testAllLoopbacks(bool loopback_result[NUM_LOOPBACKS][NUM_LOOPBACKS], bool loopback_off_result[NUM_LOOPBACKS]);

    protected:
        void run();

    private:
        void runUpdate();
        void sensorTestUpdate();

        int8_t findFlapIndex(uint8_t character);
        // void disableAll(const char* message);

        // TODO: move to serial task
        char recv_buffer[NUM_MODULES];
        void dumpStatus(void);

        // TODO: rename to match style guide
        bool pending_move_response = true;
        bool pending_no_op = false;
        uint8_t recv_count = 0;
        bool was_stopped = false;
        uint32_t stopped_at_millis = 0;

        // bool disabled = false;
        uint32_t last_sensor_print_millis_ = 0;
        bool sensor_test_ = SENSOR_TEST;

        SemaphoreHandle_t module_semaphore_;

        SemaphoreHandle_t state_semaphore_;
        // Cached state. Protected by state_semaphore_
        SplitflapState state_cache_;
        void updateStateCache();
};
