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
#include <lwip/apps/sntp.h>
#include <json11.hpp>
#include <time.h>

#include "secrets.h"

using namespace json11;

// About this example:
// - Fetches current weather data for an area in San Francisco (updating infrequently)
// - Cycles between showing the temperature and the wind speed on the split-flaps (cycles frequently)
//
// Make sure to set up secrets.h - see secrets.h.example for more.
//
// What this example demonstrates:
// - a simple JSON GET request (see fetchData)
// - json response parsing using json11 (see handleData)
// - cycling through messages at a different interval than data is loaded (see run)

// Update data every 10 minutes
#define REQUEST_INTERVAL_MILLIS (10 * 60 * 1000)

// Cycle the message that's showing more frequently, every 30 seconds (exaggerated for example purposes)
#define MESSAGE_CYCLE_INTERVAL_MILLIS (30 * 1000)

// Don't show stale data if it's been too long since successful data load
#define STALE_TIME_MILLIS (REQUEST_INTERVAL_MILLIS * 3)

// Public token for synoptic data api (it's not secret, but please don't abuse it)
#define SYNOPTICDATA_TOKEN "e763d68537d9498a90fa808eb9d415d9"

// Timezone for local time strings; this is America/Los_Angeles. See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define TIMEZONE "PST8PDT,M3.2.0,M11.1.0"

bool HTTPTask::fetchData() {
    char buf[200];
    uint32_t start = millis();
    HTTPClient http;

    // Construct the http request
    http.begin("https://api.synopticdata.com/v2/stations/latest?&token=" SYNOPTICDATA_TOKEN "&within=30&units=english&vars=air_temp,wind_speed&varsoperator=and&radius=37.765157,-122.419702,4&limit=20&fields=stid");

    // If you wanted to add headers, you would do so like this:
    // http.addHeader("Accept", "application/json");

    // Send the request as a GET
    logger_.log("Sending request");
    int http_code = http.GET();

    snprintf(buf, sizeof(buf), "Finished request in %lu millis.", millis() - start);
    logger_.log(buf);
    if (http_code > 0) {
        String data = http.getString();
        http.end();

        snprintf(buf, sizeof(buf), "Response code: %d Data length: %d", http_code, data.length());
        logger_.log(buf);

        std::string err;
        Json json = Json::parse(data.c_str(), err);

        if (err.empty()) {
            return handleData(json);
        } else {
            snprintf(buf, sizeof(buf), "Error parsing response! %s", err.c_str());
            logger_.log(buf);
            return false;
        }
    } else {
        snprintf(buf, sizeof(buf), "Error on HTTP request (%d): %s", http_code, http.errorToString(http_code).c_str());
        logger_.log(buf);
        http.end();
        return false;
    }
}

bool HTTPTask::handleData(Json json) {
    // Extract data from the json response. You could use ArduinoJson, but I find json11 to be much
    // easier to use albeit not optimized for a microcontroller.

    // Example data:
    /*
        {
            ...
            "STATION": [
                {
                    "STID": "F4637",
                    "OBSERVATIONS": {
                        "wind_speed_value_1": {
                            "date_time": "2021-11-30T23:25:00Z",
                            "value": 0.87
                        },
                        "air_temp_value_1": {
                            "date_time": "2021-11-30T23:25:00Z",
                            "value": 69
                        }
                    },
                    ...
                },
                {
                    "STID": "C5988",
                    "OBSERVATIONS": {
                        "wind_speed_value_1": {
                            "date_time": "2021-11-30T23:24:00Z",
                            "value": 1.74
                        },
                        "air_temp_value_1": {
                            "date_time": "2021-11-30T23:24:00Z",
                            "value": 68
                        }
                    },
                    ...
                },
                ...
            ]
        }
    */

   // Validate json structure and extract data:
    auto station = json["STATION"];
    if (!station.is_array()) {
        logger_.log("Parse error: STATION");
        return false;
    }
    auto station_array = station.array_items();

    std::vector<double> temps;
    std::vector<double> wind_speeds;

    for (uint8_t i = 0; i < station_array.size(); i++) {
        auto item = station_array[i];
        if (!item.is_object()) {
            logger_.log("Bad station item, ignoring");
            continue;
        }
        auto observations = item["OBSERVATIONS"];
        if (!observations.is_object()) {
            logger_.log("Bad station observations, ignoring");
            continue;
        }

        auto air_temp_value = observations["air_temp_value_1"];
        if (!air_temp_value.is_object()) {
            logger_.log("Bad air_temp_value_1, ignoring");
            continue;
        }
        auto value = air_temp_value["value"];
        if (!value.is_number()) {
            logger_.log("Bad air temp, ignoring");
            continue;
        }
        temps.push_back(value.number_value());

        auto wind_speed_value = observations["wind_speed_value_1"];
        if (!wind_speed_value.is_object()) {
            logger_.log("Bad wind_speed_value_1, ignoring");
            continue;
        }
        value = wind_speed_value["value"];
        if (!value.is_number()) {
            logger_.log("Bad wind speed, ignoring");
            continue;
        }
        wind_speeds.push_back(value.number_value());
    }

    auto entries = temps.size();
    if (entries == 0) {
        logger_.log("No data found");
        return false;
    }

    // Calculate medians
    std::sort(temps.begin(), temps.end());
    std::sort(wind_speeds.begin(), wind_speeds.end());
    double median_temp;
    double median_wind_speed;
    if ((entries % 2) == 0) {
        median_temp = (temps[entries/2 - 1] + temps[entries/2]) / 2;
        median_wind_speed = (wind_speeds[entries/2 - 1] + wind_speeds[entries/2]) / 2;
    } else {
        median_temp = temps[entries/2];
        median_wind_speed = wind_speeds[entries/2];
    }

    char buf[200];
    snprintf(buf, sizeof(buf), "Medians from %d stations: temp=%dºF, wind speed=%d knots", entries, (int)median_temp, (int)median_wind_speed);
    logger_.log(buf);

    // Construct the messages to display
    messages_.clear();

    snprintf(buf, sizeof(buf), "%d f", (int)median_temp);
    messages_.push_back(String(buf));

    snprintf(buf, sizeof(buf), "%d mph", (int)(median_wind_speed * 1.151));
    messages_.push_back(String(buf));

    // Show the data fetch time on the LCD
    time_t now;
    time(&now);
    strftime(buf, sizeof(buf), "Data: %Y-%m-%d %H:%M:%S", localtime(&now));
    display_task_.setMessage(0, String(buf));
    return true;
}


HTTPTask::HTTPTask(SplitflapTask& splitflap_task, DisplayTask& display_task, Logger& logger, const uint8_t task_core) :
        Task("HTTP", 8192, 1, task_core),
        splitflap_task_(splitflap_task),
        display_task_(display_task),
        logger_(logger),
        wifi_client_() {
}

void HTTPTask::connectWifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    char buf[256];

    logger_.log("Establishing connection to WiFi..");
    snprintf(buf, sizeof(buf), "Wifi connecting to %s", WIFI_SSID);
    display_task_.setMessage(1, String(buf));
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    snprintf(buf, sizeof(buf), "Connected to network %s", WIFI_SSID);
    logger_.log(buf);

    // Sync SNTP
    sntp_setoperatingmode(SNTP_OPMODE_POLL);

    char server[] = "time.nist.gov"; // sntp_setservername takes a non-const char*, so use a non-const variable to avoid warning
    sntp_setservername(0, server);
    sntp_init();

    logger_.log("Waiting for NTP time sync...");
    snprintf(buf, sizeof(buf), "Syncing NTP time via %s...", server);
    display_task_.setMessage(1, String(buf));
    time_t now;
    while (time(&now),now < 1625099485) {
        delay(1000);
    }

    setenv("TZ", TIMEZONE, 1);
    tzset();
    strftime(buf, sizeof(buf), "Got time: %Y-%m-%d %H:%M:%S", localtime(&now));
    logger_.log(buf);
}

void HTTPTask::run() {
    char buf[max(NUM_MODULES + 1, 200)];

    connectWifi();

    bool stale = false;
    while(1) {
        long now = millis();

        bool update = false;
        if (http_last_request_time_ == 0 || now - http_last_request_time_ > REQUEST_INTERVAL_MILLIS) {
            if (fetchData()) {
                http_last_success_time_ = millis();
                stale = false;
                update = true;
            }
            http_last_request_time_ = millis();
        }

        if (!stale && http_last_success_time_ > 0 && millis() - http_last_success_time_ > STALE_TIME_MILLIS) {
            stale = true;
            messages_.clear();
            messages_.push_back("stale");
            update = true;
        }

        if (update || now - last_message_change_time_ > MESSAGE_CYCLE_INTERVAL_MILLIS) {
            if (current_message_index_ >= messages_.size()) {
                current_message_index_ = 0;
            }

            if (messages_.size() > 0) {
                String message = messages_[current_message_index_].c_str();

                snprintf(buf, sizeof(buf), "Cycling to next message: %s", message.c_str());
                logger_.log(buf);

                // Pad message for display
                size_t len = strlcpy(buf, message.c_str(), sizeof(buf));
                memset(buf + len, ' ', sizeof(buf) - len);

                splitflap_task_.showString(buf, NUM_MODULES, false);
            }

            current_message_index_++;
            last_message_change_time_ = millis();
        }

        String wifi_status;
        switch (WiFi.status()) {
            case WL_IDLE_STATUS:
                wifi_status = "Idle";
                break;
            case WL_NO_SSID_AVAIL:
                wifi_status = "No SSID";
                break;
            case WL_CONNECTED:
                wifi_status = String(WIFI_SSID) + " " + WiFi.localIP().toString();
                break;
            case WL_CONNECT_FAILED:
                wifi_status = "Connection failed";
                break;
            case WL_CONNECTION_LOST:
                wifi_status = "Connection lost";
                break;
            case WL_DISCONNECTED:
                wifi_status = "Disconnected";
                break;
            default:
                wifi_status = "Unknown";
                break;
        }
        display_task_.setMessage(1, String("Wifi: ") + wifi_status);

        delay(1000);
    }
}
#endif
