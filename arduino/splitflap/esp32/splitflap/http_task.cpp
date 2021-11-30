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
#if HTTP
#include "http_task.h"

#include <HTTPClient.h>
#include <json11.hpp>

#include "secrets.h"

using namespace json11;

// About this example:
// - Fetches current weather data from weather.gov for a Downtown San Francisco weather station
// - Demonstrates a simple JSON GET request with custom headers (see fetchData)
// - Demonstrates json response parsing using json11 (see handleData)
// - Demonstrates cycling through messages and re-requesting data periodically (see run)

// Update every 10 minutes
#define REQUEST_INTERVAL_MILLIS (10 * 60 * 1000)

void HTTPTask::fetchData() {
    char buf[200];
    uint32_t start = millis();
    http_last_request_time_ = start;
    HTTPClient http;

    snprintf(buf, sizeof(buf), "Free heap: %u, block: %u", ESP.getFreeHeap(), ESP.getMaxAllocHeap());
    logger_.log(buf);

    // Construct the http request
    http.begin("https://api.weather.gov/stations/SFOC1/observations/latest");
    http.addHeader("User-Agent", "(splitflap example ESP32)");
    http.addHeader("Accept", "application/ld+json");

    // Send the request
    logger_.log("Sending request");
    int http_code = http.GET();

    snprintf(buf, sizeof(buf), "Finished request in %lu millis.", millis() - start);
    logger_.log(buf);
    if (http_code > 0) {
        snprintf(buf, sizeof(buf), "Response code: %d", http_code);
        logger_.log(buf);

        String data = http.getString();
        http.end();
        
        std::string err;
        Json json = Json::parse(data.c_str(), err);

        if (err.empty()) {
            logger_.log(json.dump().c_str());
            handleData(json);
        } else {
            snprintf(buf, sizeof(buf), "Error parsing response! %s", err.c_str());
            logger_.log(buf);
        }
    } else {
        snprintf(buf, sizeof(buf), "Error on HTTP request (%d): %s", http_code, http.errorToString(http_code).c_str());
        logger_.log(buf);
        http.end();
    }
    http_last_request_time_ = millis();
}

void HTTPTask::handleData(Json json) {
    // Extract data from the json response. You could use ArduinoJson, but I find json11 to be much
    // easier to use albeit not optimized for a microcontroller.

    char buf[200];

    // Example data:
    /*
        {
            ...
            "station": "https://api.weather.gov/stations/SFOC1",
            "timestamp": "2021-11-30T08:00:00+00:00",
            ...
            "temperature": {
                "unitCode": "wmoUnit:degC",
                "value": 11.109999999999999,
                "qualityControl": "V"
            },
            ...
        }
    */

   // Validate json structure and extract data:
    auto temperature = json["temperature"];
    if (!temperature.is_object()) {
        logger_.log("Parse error: temperature");
        return;
    }
    auto unit_code = temperature["unitCode"];
    if (!unit_code.is_string()) {
        logger_.log("Parse error: unitCode");
        return;
    }
    auto value = temperature["value"];
    if (!value.is_number()) {
        logger_.log("Parse error: value");
        return;
    }

    // Make sure units are degrees C as expected
    if (unit_code.string_value() != "wmoUnit:degC") {
        snprintf(buf, sizeof(buf), "Bad unitCode: %s", unit_code.string_value().c_str());
        logger_.log(buf);
        return;
    }

    // Convert value to sane units ;-)
    double fahrenheit = value.number_value() * 9 / 5 + 32;

    // Display the string directly
    int len = snprintf(buf, sizeof(buf), "%d f", (int)fahrenheit);
    splitflap_task_.showString(buf, len);
}


HTTPTask::HTTPTask(SplitflapTask& splitflap_task, Logger& logger, const uint8_t task_core) :
        Task("HTTP", 8192, 1, task_core),
        splitflap_task_(splitflap_task),
        logger_(logger),
        wifi_client_() {

}

void HTTPTask::connectWifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        logger_.log("Establishing connection to WiFi..");
    }

    char buf[256];
    snprintf(buf, sizeof(buf), "Connected to network %s", WIFI_SSID);
    logger_.log(buf);
}

void HTTPTask::run() {
    connectWifi();

    while(1) {
        long now = millis();
        if (http_last_request_time_ == 0 || now - http_last_request_time_ > REQUEST_INTERVAL_MILLIS) {
            fetchData();
        }
        delay(1000);
    }
}
#endif
