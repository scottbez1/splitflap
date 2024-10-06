#pragma once

#include <Arduino.h>

class Result {
    public:
        enum class Code : uint8_t {
            PASS = 0,
            FAIL = 1,
            ABORT = 2,
        };
        const Code result_code_;
        const String message_;

        bool canContinue() {
            return result_code_ == Code::PASS;
        }

        static Result pass(String message) {
            return Result(Code::PASS, message);
        }

        static Result fail(String message) {
            return Result(Code::FAIL, message);
        }

        static Result abort(String message) {
            return Result(Code::ABORT, message);
        }

    private:
        Result(Code code, String message) : result_code_(code), message_(message) {};
};
