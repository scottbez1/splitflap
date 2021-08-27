/*
   Copyright 2020 Scott Bezek and the splitflap contributors

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

class RecursiveSemaphoreGuard {
    public:
        RecursiveSemaphoreGuard(SemaphoreHandle_t handle) : handle_{handle} {
            xSemaphoreTakeRecursive(handle_, portMAX_DELAY);
        }
        ~RecursiveSemaphoreGuard() {
            xSemaphoreGiveRecursive(handle_);
        }
        RecursiveSemaphoreGuard(RecursiveSemaphoreGuard const&)=delete;
        RecursiveSemaphoreGuard& operator=(RecursiveSemaphoreGuard const&)=delete;

    private:
        SemaphoreHandle_t handle_;
};
