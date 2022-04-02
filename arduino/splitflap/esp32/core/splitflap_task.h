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
#include "logger.h"
#include "src/splitflap_module_data.h"

#include "task.h"

enum class SplitflapMode {
    MODE_RUN,
    MODE_SENSOR_TEST,
};

struct SplitflapModuleState {
    State state;
    uint8_t flap_index;
    bool moving;
    bool home_state;
    uint8_t count_unexpected_home;
    uint8_t count_missed_home;

    bool operator==(const SplitflapModuleState& other) {
        return state == other.state
            && flap_index == other.flap_index
            && moving == other.moving
            && home_state == other.home_state
            && count_unexpected_home == other.count_unexpected_home
            && count_missed_home == other.count_missed_home;
    }

    bool operator!=(const SplitflapModuleState& other) {
        return !(*this == other);
    }
};

struct SplitflapState {
    SplitflapMode mode;
    SplitflapModuleState modules[NUM_MODULES];

#ifdef CHAINLINK
    bool loopbacks_ok = false;
#endif

    bool operator==(const SplitflapState& other) {
        for (uint8_t i = 0; i < NUM_MODULES; i++) {
            if (modules[i] != other.modules[i]) {
                return false;
            }
        }

        return mode == other.mode
#ifdef CHAINLINK
            && loopbacks_ok == other.loopbacks_ok
#endif
            ;
    }

    bool operator!=(const SplitflapState& other) {
        return !(*this == other);
    }
};

enum class LedMode {
    AUTO,
    MANUAL,
};

enum class CommandType {
    MODULES,
    SENSOR_TEST_SET,
    SENSOR_TEST_CLEAR,
    CONFIG,
};

struct ModuleConfig {
    uint8_t target_flap_index;
    uint8_t movement_nonce;
    uint8_t reset_nonce;
};

struct ModuleConfigs {
    ModuleConfig config[NUM_MODULES];
};

struct Command {
    CommandType command_type;
    union CommandData {
        uint8_t module_command[NUM_MODULES];
        ModuleConfigs module_configs;
    };
    CommandData data;
};

#define QCMD_NO_OP          0
#define QCMD_RESET_AND_HOME 1
#define QCMD_LED_ON         2
#define QCMD_LED_OFF        3
#define QCMD_DISABLE        4
#define QCMD_FLAP           5

class SplitflapTask : public Task<SplitflapTask> {
    friend class Task<SplitflapTask>; // Allow base Task to invoke protected run()

    public:
        SplitflapTask(const uint8_t task_core, const LedMode led_mode);
        ~SplitflapTask();
        
        SplitflapState getState();

        void showString(const char *str, uint8_t length, bool force_full_rotation = FORCE_FULL_ROTATION);
        void resetAll();
        void disableAll();
        void setLed(uint8_t id, bool on);
        void setSensorTest(bool sensor_test);
        void setLogger(Logger* logger);
        void postRawCommand(Command command);

    protected:
        void run();

    private:
        const LedMode led_mode_;
        const SemaphoreHandle_t state_semaphore_;
        QueueHandle_t queue_;
        Command queue_receive_buffer_ = {};
        Logger* logger_;

        bool all_stopped_ = true;

        uint32_t last_sensor_print_millis_ = 0;
        bool sensor_test_ = SENSOR_TEST;
        ModuleConfigs current_configs_ = {};

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
        void log(const char* msg);

        int8_t findFlapIndex(uint8_t character);
};
