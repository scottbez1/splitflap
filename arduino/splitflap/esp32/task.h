#pragma once

#include<Arduino.h>

// Static polymorphic abstract base class for a FreeRTOS task using CRTP pattern. Concrete implementations
// should implement a run() method.
// Inspired by https://fjrg76.wordpress.com/2018/05/23/objectifying-task-creation-in-freertos-ii/
template<class T>
class Task {
    public:
        Task(const char* name, uint32_t stackDepth, UBaseType_t priority, const BaseType_t coreId = tskNO_AFFINITY) : 
                name { name },
                stackDepth {stackDepth},
                priority { priority },
                coreId { coreId }
        {}

        TaskHandle_t getHandle() {
            return taskHandle;
        }

        void begin() {
            BaseType_t result = xTaskCreatePinnedToCore(taskFunction, name, stackDepth, this, priority, &taskHandle, coreId);
            assert("Failed to create task" && result == pdPASS);
        }

    private:
        static void taskFunction(void* params) {
            T* t = static_cast<T*>(params);
            t->run();
        }

        const char* name;
        uint32_t stackDepth;
        UBaseType_t priority;
        TaskHandle_t taskHandle;
        const BaseType_t coreId;
};
