
#include <WiFi.h>
#include <HTTPClient.h>

#include "json11.hpp"
#include "firestore.h"

const String FIRESTORE_BASE_URL = "https://firestore.googleapis.com/v1";

using namespace json11;

Firestore::Firestore(String project_id, Jwt jwt) : project_id_(project_id), jwt_(jwt) {

}

DynamicJsonDocument Firestore::get(String path, size_t json_capacity=1024) {
    HTTPClient http;
    http.begin(FIRESTORE_BASE_URL + "/projects/" + project_id_ + "/databases/(default)/documents/" + path);
    http.addHeader("Authorization", "Bearer " + jwt_.get());
    int http_code = http.GET();
    if (http_code > 0) {
        Serial.println(http_code);
        
        std::string err;
        Json json = Json::parse(http.getString().c_str(), err);

        if (err.empty()) {
            Serial.println(json.dump().c_str());
        } else {
            Serial.printf("Error parsing response! %s", err.c_str());
        }

        DynamicJsonDocument result(json_capacity);
        // deserializeJson(result, http.getString());
        return result;
    } else {
        Serial.println("Error on HTTP request");
    }
    http.end();
    return DynamicJsonDocument(0);
}

int Firestore::set(String path, JsonDocument data) {
    DynamicJsonDocument request_data(1024);

    JsonObject update = request_data.createNestedObject("update");
    // update.set(data);
    
    String request_data_str;
    serializeJson(request_data, request_data_str);

    HTTPClient http;
    http.begin(FIRESTORE_BASE_URL + "/projects/" + project_id_ + "/databases/(default)/documents/" + path);
    http.addHeader("Authorization", "Bearer " + jwt_.get());
    int http_code = http.POST(request_data_str);
    Serial.println(http_code);
    Serial.println(http.getString());
    http.end();
    return http_code;
}
