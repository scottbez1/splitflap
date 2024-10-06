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
#include <FFat.h>

#include "pb_decode.h"
#include "pb_encode.h"

#include "../proto_gen/splitflap.pb.h"

#include "config.h"
#include "configuration.h"
#include "semaphore_guard.h"

static const char* CONFIG_PATH = "/config.pb";

Configuration::Configuration() {
  mutex_ = xSemaphoreCreateMutex();
  assert(mutex_ != NULL);
}

Configuration::~Configuration() {
  vSemaphoreDelete(mutex_);
}

bool Configuration::loadFromDisk() {
    SemaphoreGuard lock(mutex_);
    FatGuard fatGuard(logger_);
    if (!fatGuard.mounted_) {
        return false;
    }

    File f = FFat.open(CONFIG_PATH);
    if (!f) {
        log("Failed to read config file");
        return false;
    }

    size_t read = f.readBytes((char*)buffer_, sizeof(buffer_));
    f.close();

    pb_istream_t stream = pb_istream_from_buffer(buffer_, read);
    if (!pb_decode(&stream, PB_PersistentConfiguration_fields, &pb_buffer_)) {
        char buf[200];
        snprintf(buf, sizeof(buf), "Decoding failed: %s", PB_GET_ERROR(&stream));
        log(buf);
        pb_buffer_ = {};
        return false;
    }

    if (pb_buffer_.version != PERSISTENT_CONFIGURATION_VERSION) {
        char buf[200];
        snprintf(buf, sizeof(buf), "Invalid config version. Expected %u, received %u", PERSISTENT_CONFIGURATION_VERSION, pb_buffer_.version);
        log(buf);
        pb_buffer_ = {};
        return false;
    }

    if (pb_buffer_.num_flaps != NUM_FLAPS) {
        char buf[200];
        snprintf(buf, sizeof(buf), "Invalid config - stored num_flaps was %u but this firmware expects %u. Ignoring saved config.", pb_buffer_.num_flaps, NUM_FLAPS);
        log(buf);
        pb_buffer_ = {};
        return false;
    }
    loaded_ = true;

    char buf[200];
    snprintf(
        buf,
        sizeof(buf),
        "Loaded %u offsets",
        pb_buffer_.module_offset_steps_count
    );
    log(buf);

    uint8_t previewN = min(pb_buffer_.module_offset_steps_count, static_cast<pb_size_t>(6));
    snprintf(
        buf,
        sizeof(buf),
        "First %u offsets:",
        previewN
    );
    log(buf);
    for (uint8_t i = 0; i < previewN; i++) {
        snprintf(
            buf,
            sizeof(buf),
            "  %u: %u",
            i,
            pb_buffer_.module_offset_steps[i]
        );
        log(buf);
    }

    return true;
}

bool Configuration::saveToDisk() {
    SemaphoreGuard lock(mutex_);

    pb_ostream_t stream = pb_ostream_from_buffer(buffer_, sizeof(buffer_));
    pb_buffer_.version = PERSISTENT_CONFIGURATION_VERSION;
    if (!pb_encode(&stream, PB_PersistentConfiguration_fields, &pb_buffer_)) {
        char buf[200];
        snprintf(buf, sizeof(buf), "Encoding failed: %s", PB_GET_ERROR(&stream));
        log(buf);
        return false;
    }

    FatGuard fatGuard(logger_);
    if (!fatGuard.mounted_) {
        return false;
    }
    File f = FFat.open(CONFIG_PATH, FILE_WRITE);
    if (!f) {
        log("Failed to read config file");
        return false;
    }
    size_t written = f.write(buffer_, stream.bytes_written);
    f.close();

    char buf[20];
    snprintf(buf, sizeof(buf), "Wrote %d bytes", written);
    log(buf);

    if (written != stream.bytes_written) {
        log("Failed to write all bytes to file");
        return false;
    }

    return true;
}

PB_PersistentConfiguration Configuration::get() {
    SemaphoreGuard lock(mutex_);
    if (!loaded_) {
        return PB_PersistentConfiguration();
    }
    return pb_buffer_;
}

bool Configuration::setModuleOffsetsAndSave(uint16_t offsets[NUM_MODULES]) {
    {
        SemaphoreGuard lock(mutex_);
        pb_buffer_.num_flaps = NUM_FLAPS;
        pb_buffer_.module_offset_steps_count = NUM_MODULES;
        for (uint8_t i = 0; i < NUM_MODULES; i++) {
            pb_buffer_.module_offset_steps[i] = offsets[i];
        }
    }
    return saveToDisk();
}

void Configuration::setLogger(Logger* logger) {
    logger_ = logger;
}

void Configuration::log(const char* msg) {
    if (logger_ != nullptr) {
        logger_->log(msg);
    }
}
