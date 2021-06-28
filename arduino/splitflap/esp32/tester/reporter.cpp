#include "reporter.h"

Reporter::Reporter() {
    
}

void Reporter::testSuiteStarted(String serial) {
    Serial.printf("Test suite started: %s\n", serial.c_str());
    // TODO
}
void Reporter::testSuiteFinished() {
    Serial.printf("Test suite finished");
    // TODO
}
void Reporter::testStarted(String id) {
    Serial.printf("Test started: %s\n", id.c_str());
    // TODO
}
void Reporter::testFinished(Result result) {
    Serial.printf("Test result: %u -> %s\n", result.result_code_, result.message_.c_str());
    // TODO
}
