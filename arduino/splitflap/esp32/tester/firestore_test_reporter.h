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

#include "firestore.h"
#include "result.h"

class FirestoreTestReporter {
    public:
        FirestoreTestReporter(Firestore firestore);

        bool checkFirestoreAccess();

        void testSuiteStarted(String serial, uint32_t test_suite_version);
        bool testSuiteFinished(Result::Code result_code);

        void testStarted(String id);
        void testFinished(Result result);

    private:
        static std::string resultCodeToString(Result::Code result_code);

        Firestore firestore_;
        json11::Json::object test_suite_document_;
        uint32_t test_suite_start_millis_;
        uint32_t test_start_millis_;
        String current_test_id_;
        json11::Json::array test_results_;
};
