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
#include <WiFi.h>
#include <HTTPClient.h>

#include "json11.hpp"
#include "firestore.h"

const String FIRESTORE_BASE_URL = "https://firestore.googleapis.com/v1/";

using namespace json11;

Firestore::Firestore(String project_id, Jwt jwt) : project_id_(project_id), jwt_(jwt) {

}

Json Firestore::get(String path, size_t json_capacity=1024) {
    uint32_t start = millis();
    HTTPClient http;
    http.begin(FIRESTORE_BASE_URL + doc_path(path));
    http.addHeader("Authorization", "Bearer " + jwt_.get());
    int http_code = http.GET();
    Serial.printf("Finished request in %lu millis.\n", millis() - start);
    if (http_code > 0) {
        Serial.println(http_code);
        
        std::string err;
        Json json = Json::parse(http.getString().c_str(), err);

        if (err.empty()) {
            Serial.println(json.dump().c_str());
        } else {
            Serial.printf("Error parsing response! %s", err.c_str());
        }
        http.end();
        return json;
    } else {
        Serial.println("Error on HTTP request");
    }
    http.end();
    return Json();
}

bool Firestore::set(String path, Json fields) {
    uint32_t start = millis();
    Json body = Json::object {
        {"writes", Json::array {
            Json::object {
                {"update", Json::object {
                    {"name", std::string(doc_path(path).c_str())},
                    {"fields", fields},
                }},
            },
        }},
    };
    String body_str = String(body.dump().c_str());
    Serial.println(body_str);

    HTTPClient http;
    http.begin(FIRESTORE_BASE_URL + base_path() + ":batchWrite");
    http.addHeader("Authorization", "Bearer " + jwt_.get());
    int http_code = http.POST(body_str);
    Serial.printf("Finished request in %lu millis.\n", millis() - start);
    if (http_code > 0) {
        Serial.println(http_code);

        std::string err;
        Json json = Json::parse(http.getString().c_str(), err);
        http.end();

        if (err.empty()) {
            // TODO: Check result for errors!
            Serial.println(json.dump().c_str());
            return http_code == 200;
        } else {
            Serial.printf("Error parsing response! %s", err.c_str());
            return false;
        }
    } else {
        http.end();
        Serial.println("Error on HTTP request");
        return false;
    }
}

const String Firestore::auto_id_chars_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

// Based on the JS auto-id implementation: https://github.com/firebase/firebase-js-sdk/blob/4f446f0a1c00f080fb58451b086efa899be97a08/packages/firestore/src/util/misc.ts#L24-L34
String Firestore::gen_auto_id() {
    String s;
    for (uint8_t i = 0; i < 20; i++) {
        s += auto_id_chars_[random(auto_id_chars_.length())];
    }
    return s;
}

String Firestore::base_path() {
    return "projects/" + project_id_ + "/databases/(default)/documents";
}

String Firestore::doc_path(String path) {
    return base_path() + "/" + path;
}
