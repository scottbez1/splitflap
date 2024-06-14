/*
   Copyright 2024 Scott Bezek and the splitflap contributors

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

#include <FFat.h>
#include <PacketSerial.h>

#include "../proto_gen/splitflap.pb.h"

#include "logger.h"

const uint32_t PERSISTENT_CONFIGURATION_VERSION = 1;

class Configuration {
    public:
        Configuration();
        ~Configuration();

        void setLogger(Logger* logger);
        bool loadFromDisk();
        bool saveToDisk();
        PB_PersistentConfiguration get();
        bool setModuleOffsetsAndSave(uint16_t offsets[NUM_MODULES]);

    private:
        SemaphoreHandle_t mutex_;

        Logger* logger_ = nullptr;
        bool loaded_ = false;
        PB_PersistentConfiguration pb_buffer_ = {};

        uint8_t buffer_[PB_PersistentConfiguration_size];

        void log(const char* msg);
};
class FatGuard {
    public:
        FatGuard(Logger* logger) : logger_(logger) {
            if (!FFat.begin(true)) {
                if (logger_ != nullptr) {
                    logger_->log("Failed to mount FFat");
                }
                return;
            }
            if (logger_ != nullptr) {
                logger_->log("Mounted FFat");
            }
            mounted_ = true;
        }
        ~FatGuard() {
            if (mounted_) {
                FFat.end();
                if (logger_ != nullptr) {
                    logger_->log("Unmounted FFat");
                }
            }
        }
        FatGuard(FatGuard const&)=delete;
        FatGuard& operator=(FatGuard const&)=delete;

        bool mounted_ = false;

    private:
        Logger* logger_;
};
