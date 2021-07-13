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

#include "firestore_test_reporter.h"

using namespace json11;

const String COLLECTION = "qcResults";

FirestoreTestReporter::FirestoreTestReporter(Firestore firestore) : firestore_(firestore) {}

std::string FirestoreTestReporter::resultCodeToString(Result::Code result_code) {
    switch (result_code) {
        case Result::Code::ABORT:
            return "ABORT";
        case Result::Code::FAIL:
            return "FAIL";
        case Result::Code::PASS:
            return "PASS";
        default:
            return "UNKNOWN";
    }
}

bool FirestoreTestReporter::checkFirestoreAccess() {
    Json r = firestore_.get("dummy/dummy", 1024);
    if (r["fields"]["test"]["integerValue"].string_value() == "123") {
        return true;
    }
    return false;
}

void FirestoreTestReporter::testSuiteStarted(String serial, uint32_t test_suite_version) {
    test_suite_start_millis_ = millis();
    test_results_.clear();
    test_suite_document_ = Json::object {
        {"serial", Json::object {
            {"stringValue", std::string(serial.c_str())},
        }},
        {"sku", Json::object {
            {"stringValue", "CD01"},
        }},
        {"start_seconds", Json::object {
            {"integerValue", std::string(String(time(nullptr)).c_str())},
        }},
        {"test_suite_version", Json::object {
            {"integerValue", std::string(String(test_suite_version).c_str())},
        }},
    };
}

bool FirestoreTestReporter::testSuiteFinished(Result::Code result_code) {
    test_suite_document_["end_seconds"] = Json::object {
        {"integerValue", std::string(String(time(nullptr)).c_str())},
    };
    test_suite_document_["duration_millis"] = Json::object {
        {"integerValue", std::string(String(millis() - test_suite_start_millis_).c_str())},
    };
    test_suite_document_["tests"] = Json::object {
        {"arrayValue", Json::object {
            {"values", test_results_},
        }},
    };
    test_suite_document_["result"] = Json::object {
        {"stringValue", resultCodeToString(result_code)},
    };

    bool result = firestore_.set(COLLECTION + "/" + Firestore::gen_auto_id(), test_suite_document_);

    test_suite_document_ = Json::object();
    return result;
}

void FirestoreTestReporter::testStarted(String id) {
    current_test_id_ = id;
    test_start_millis_ = millis();
}

void FirestoreTestReporter::testFinished(Result result) {
    test_results_.push_back(Json::object {
        {"mapValue", Json::object {
            {"fields", Json::object {
                {"id", Json::object {
                    {"stringValue", std::string(current_test_id_.c_str())},
                }},
                {"result", Json::object {
                    {"stringValue", resultCodeToString(result.result_code_)},
                }},
                {"message", Json::object {
                    {"stringValue", std::string(result.message_.c_str())},
                }},
                {"duration_millis", Json::object {
                    {"integerValue", std::string(String(millis() - test_start_millis_).c_str())},
                }},
            }},
        }},
    });
    current_test_id_ = "";
    test_start_millis_ = 0;
}
