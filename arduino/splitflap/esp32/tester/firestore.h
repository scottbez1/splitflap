#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "jwt.h"

class Firestore {

    public:
        Firestore(String project_id, Jwt jwt);

        DynamicJsonDocument get(String path, size_t json_capacity);
        int set(String path, JsonDocument data);

    private:
        String project_id_;
        Jwt jwt_;
};
