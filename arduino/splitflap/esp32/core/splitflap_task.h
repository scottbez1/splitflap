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
    uint8_t flap_index;
    bool moving;
    uint8_t count_unexpected_home;
    uint8_t count_missed_home;
};

struct SplitflapState {
    SplitflapModuleState modules[NUM_MODULES];

#ifdef CHAINLINK
    bool loopbacks_ok = false;
#endif
};

enum class LedMode {
    AUTO,
    MANUAL,
};

struct Command {
    uint8_t data[NUM_MODULES];
};

class SplitflapTask : public Task<SplitflapTask> {
    friend class Task<SplitflapTask>; // Allow base Task to invoke protected run()

    public:
        SplitflapTask(const uint8_t task_core, const LedMode led_mode);
        ~SplitflapTask();
        
        SplitflapState getState();

        void showString(const char *str, uint8_t length);
        void resetAll();
        void setLed(uint8_t id, bool on);

    protected:
        void run();

    private:
        const LedMode led_mode_;
        const SemaphoreHandle_t state_semaphore_;
        QueueHandle_t queue_;
        Command queue_receive_buffer_ = {};

        // TODO: move to serial task
        char recv_buffer[NUM_MODULES];
        uint8_t recv_count = 0;
        void dumpStatus(void);

        // TODO: rename to match style guide
        bool pending_move_response = true;
        bool pending_no_op = false;
        bool was_stopped = false;
        uint32_t stopped_at_millis = 0;

        // bool disabled = false;
        uint32_t last_sensor_print_millis_ = 0;
        bool sensor_test_ = SENSOR_TEST;

#ifdef CHAINLINK
        uint8_t loopback_current_out_index_ = 0;
        uint16_t loopback_step_index_ = 0;
        bool loopback_current_ok_ = true;
        bool loopback_all_ok_ = false;
#endif

        // Cached state. Protected by state_semaphore_
        SplitflapState state_cache_;
        void updateStateCache();

        void processQueue();
        void runUpdate();
        void sensorTestUpdate();

        int8_t findFlapIndex(uint8_t character);
        // void disableAll(const char* message);
};
