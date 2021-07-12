#pragma once

#include <Arduino.h>

#include "json11.hpp"
#include "jwt.h"

class Firestore {

    public:
        Firestore(String project_id, Jwt jwt);

        json11::Json get(String path, size_t json_capacity);
        bool set(String path, json11::Json fields);
        static String gen_auto_id();

    private:
        static const String auto_id_chars_;

        String project_id_;
        Jwt jwt_;

        String base_path();
        String doc_path(String path);
};
