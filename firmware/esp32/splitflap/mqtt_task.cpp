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
#if MQTT
#include <ArduinoOTA.h>

#include "mqtt_task.h"
#include "secrets.h"

using namespace json11;

// This MQTT demo assumes a home assistant MQTT instance, and will do automatic registration
// via the home assistant config topic when it comes online and connects to MQTT. You shouldn't
// need to change any of this to get it to work with home assistant.
#define MQTT_CONFIG_TOPIC "homeassistant/text/" DEVICE_INSTANCE_NAME "/config"
#define MQTT_COMMAND_TOPIC "home/" DEVICE_INSTANCE_NAME "/command"
#define MQTT_STATE_TOPIC "home/" DEVICE_INSTANCE_NAME "/state"
#define MQTT_AVAILABILITY_TOPIC "home/" DEVICE_INSTANCE_NAME "/availability"

MQTTTask::MQTTTask(SplitflapTask& splitflap_task, DisplayTask& display_task, Logger& logger, const uint8_t task_core) :
        Task("MQTT", 8192, 1, task_core),
        splitflap_task_(splitflap_task),
        display_task_(display_task),
        logger_(logger),
        wifi_client_(),
        mqtt_client_(wifi_client_) {
    auto callback = [this](char *topic, byte *payload, unsigned int length) { mqttCallback(topic, payload, length); };
    mqtt_client_.setCallback(callback);
}

void MQTTTask::connectWifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // Disable WiFi sleep as it causes glitches on pin 39; see https://github.com/espressif/arduino-esp32/issues/4903#issuecomment-793187707
    WiFi.setSleep(WIFI_PS_NONE);

    char buf[256];

    snprintf(buf, sizeof(buf), "Wifi connecting to %s", WIFI_SSID);
    display_task_.setMessage(0, String(buf));

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        logger_.log("Establishing connection to WiFi..");
    }

    snprintf(buf, sizeof(buf), "Connected to network %s", WIFI_SSID);
    logger_.log(buf);

    snprintf(buf, sizeof(buf), "Wifi IP: %s", WiFi.localIP().toString().c_str());
    display_task_.setMessage(0, String(buf));
}

void MQTTTask::mqttCallback(char *topic, byte *payload, unsigned int length) {
    char buf[256];
    snprintf(buf, sizeof(buf), "Received mqtt callback for topic %s, length %u", topic, length);
    logger_.log(buf);


    splitflap_task_.showString((const char *)payload, length, false, true);
}

void MQTTTask::connectMQTT() {
    char buf[400];
    mqtt_client_.setServer(MQTT_SERVER, MQTT_PORT);
    logger_.log("Attempting MQTT connection...");
    snprintf(buf, sizeof(buf), "MQTT connecting to %s:%d", MQTT_SERVER, MQTT_PORT);
    display_task_.setMessage(1, String(buf));

    if (mqtt_client_.connect(DEVICE_INSTANCE_NAME, MQTT_USER, MQTT_PASSWORD, MQTT_AVAILABILITY_TOPIC, 1, true, "offline")) {
        logger_.log("MQTT connected");
        mqtt_client_.subscribe(MQTT_COMMAND_TOPIC);

        // TODO: I believe it's possible to do more complex config to register as a device with multiple
        // entities; it'd be great to explore additional entities like a display backlight control,
        // detailed module status info, etc. Though at a certain point it may be preferable to make the
        // splitflap library an ESPHome external component for even more options, easier programming,
        // and more customization than MQTT offers...
        Json config = Json::object {
            { "name", DEVICE_INSTANCE_NAME },
            { "command_topic", MQTT_COMMAND_TOPIC },
            { "state_topic", MQTT_STATE_TOPIC },
            { "availability_topic", MQTT_AVAILABILITY_TOPIC },
            { "payload_available", "online" },
            { "payload_not_available", "offline" },
            { "unique_id", DEVICE_INSTANCE_NAME },
            { "max", NUM_MODULES },
        };
        std::string json_str = config.dump();
        boolean result = mqtt_client_.publish(MQTT_CONFIG_TOPIC, json_str.c_str());
        snprintf(buf, sizeof(buf), "Result of publish: %d", result);
        logger_.log(buf);
        logger_.log("Published MQTT discovery message");
        logger_.log(json_str.c_str());

        mqtt_client_.publish(MQTT_AVAILABILITY_TOPIC, "online", true);

        snprintf(buf, sizeof(buf), "MQTT connected! (%s:%d)", MQTT_SERVER, MQTT_PORT);
        display_task_.setMessage(1, String(buf));
    } else {
        snprintf(buf, sizeof(buf), "MQTT failed rc=%d will try again in 5 seconds", mqtt_client_.state());
        logger_.log(buf);

        snprintf(buf, sizeof(buf), "MQTT failed rc=%d", mqtt_client_.state());
        display_task_.setMessage(1, String(buf));
    }
}

void MQTTTask::run() {
    char buf[256];
    display_task_.setMessage(0, "");
    display_task_.setMessage(1, "");
    connectWifi();
    connectMQTT();

    ArduinoOTA
        .onStart([this]() {
            if (ArduinoOTA.getCommand() == U_FLASH) {
                logger_.log("Start OTA (flash)");
            } else { // U_SPIFFS
                logger_.log("Start OTA (filesystem)");
                // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            }
        })
        .onEnd([this]() {
            logger_.log("OTA End");
        })
        .onProgress([this](unsigned int progress, unsigned int total) {
            char buf2[256];
            static uint32_t last_progress;
            if (millis() - last_progress > 1000) {
                snprintf(buf2, sizeof(buf2), "OTA Progress: %d%%", (int)(progress * 100 / total));
                logger_.log(buf2);
                last_progress = millis();
            }
        })
        .onError([this](ota_error_t error) {
            char buf2[256];
            snprintf(buf2, sizeof(buf2), "OTA Error: %u", error);
            logger_.log(buf2);
            if (error == OTA_AUTH_ERROR) logger_.log("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) logger_.log("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) logger_.log("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) logger_.log("Receive Failed");
            else if (error == OTA_END_ERROR) logger_.log("End Failed");
        })
        .setHostname(DEVICE_INSTANCE_NAME)
        .setPassword(OTA_PASSWORD)
        .begin();

    wl_status_t wifi_last_status = WL_DISCONNECTED;
    uint32_t last_state_publish = 0;
    SplitflapState last_state = {};
    uint32_t last_availability_publish = 0;
    while(1) {
        long now = millis();
        wl_status_t wifi_new_status = WiFi.status();
        if (wifi_new_status != wifi_last_status) {
            if (wifi_new_status == WL_CONNECTED) {
                snprintf(buf, sizeof(buf), "Wifi IP: %s", WiFi.localIP().toString().c_str());
                display_task_.setMessage(0, String(buf));
            } else {
                snprintf(buf, sizeof(buf), "Wifi connecting to %s", WIFI_SSID);
                display_task_.setMessage(0, String(buf));
            }
            wifi_last_status = wifi_new_status;
        }
        if (!mqtt_client_.connected() && (now - mqtt_last_connect_time_) > 5000) {
            logger_.log("Reconnecting MQTT");
            mqtt_last_connect_time_ = now;
            connectMQTT();
        }
        if (mqtt_client_.connected()) {
            SplitflapState state = splitflap_task_.getState();
            if (state != last_state) {
                char flap_buf[NUM_MODULES+1];
                bool all_idle = true;
                for (uint8_t i = 0; i < NUM_MODULES; i++) {
                    flap_buf[i] = flaps[state.modules[i].flap_index];
                    if (state.modules[i].moving) {
                        all_idle = false;
                    }
                }
                flap_buf[NUM_MODULES] = 0;

                if (all_idle && (now - last_state_publish) > 200) {
                    last_state = state;
                    last_state_publish = now;
                    snprintf(buf, sizeof(buf), "Publishing state: %s", flap_buf);
                    logger_.log(buf);
                    mqtt_client_.publish(MQTT_STATE_TOPIC, flap_buf);
                }
            }
            if (now > last_availability_publish + 1800000) {
                mqtt_client_.publish(MQTT_AVAILABILITY_TOPIC, "online", true);
                last_availability_publish = now;
            }
        }
        mqtt_client_.loop();
        ArduinoOTA.handle();
        delay(1);
    }
}

#endif
