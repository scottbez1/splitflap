#include <Arduino.h>

#include "result.h"

class Reporter {

    public:
        Reporter();

        void testSuiteStarted(String serial);
        void testSuiteFinished();
        
        void testStarted(String id);
        void testFinished(Result result);

    private:
        
};
