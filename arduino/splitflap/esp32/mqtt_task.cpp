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
#ifdef MQTT
#include "mqtt_task.h"
#include "secrets.h"

void MQTTTask::connectWifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Establishing connection to WiFi..");
    }

    Serial.printf("Connected to network %s\n", WIFI_SSID);
}

void MQTTTask::mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.printf("Received mqtt callback for topic %s\n", topic);
    splitflap_task_.showString((const char *)payload, length);
}

void MQTTTask::connectMQTT() {
    mqtt_client_.setServer(MQTT_SERVER, 1883);
    Serial.printf("Attempting MQTT connection...\n");
    if (mqtt_client_.connect(HOSTNAME, MQTT_USER, MQTT_PASSWORD)) {
        Serial.printf("MQTT connected\n");
        mqtt_client_.subscribe(MQTT_COMMAND_TOPIC);
    } else {
        Serial.printf("MQTT failed rc=%d will try again in 5 seconds\n", mqtt_client_.state());
    }
}

MQTTTask::MQTTTask(SplitflapTask& splitflap_task, const uint8_t task_core) : Task{"MQTT", 8192, 1, task_core}, splitflap_task_{splitflap_task}, wifi_client_(), mqtt_client_(wifi_client_) {
    auto callback = [this](char *topic, byte *payload, unsigned int length) { mqttCallback(topic, payload, length); };
    mqtt_client_.setCallback(callback);
}

void MQTTTask::run() {
    connectWifi();
    connectMQTT();

    while(1) {
        long now = millis();
        if (!mqtt_client_.connected() && (now - mqtt_last_connect_time_) > 5000) {
            Serial.printf("Reconnecting MQTT\n");
            mqtt_last_connect_time_ = now;
            connectMQTT();
        }
        mqtt_client_.loop();
        delay(1);
    }
}
#endif
