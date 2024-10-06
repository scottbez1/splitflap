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
#include <json11.hpp>
#include <WiFi.h>

#include "../core/logger.h"
#include "../core/splitflap_task.h"
#include "../core/task.h"

#include "display_task.h"

class HTTPTask : public Task<HTTPTask> {
    friend class Task<HTTPTask>; // Allow base Task to invoke protected run()

    public:
        HTTPTask(SplitflapTask& splitflap_task, DisplayTask& display_task, Logger& logger, const uint8_t task_core);

    protected:
        void run();

    private:
        void connectWifi();
        bool fetchData();
        bool handleData(json11::Json json);

        SplitflapTask& splitflap_task_;
        DisplayTask& display_task_;
        Logger& logger_;
        WiFiClient wifi_client_;
        uint32_t http_last_request_time_ = 0;
        uint32_t http_last_success_time_ = 0;

        std::vector<String> messages_ = {};
        uint8_t current_message_index_ = 0;
        uint32_t last_message_change_time_ = 0;

};
