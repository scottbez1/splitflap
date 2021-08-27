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
