// Copyright 2024 ESP RAII Library Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

// Main header file for ESP RAII Library
// Include all RAII wrappers

#include "esp_raii_lib/gpio.hpp"
#include "esp_raii_lib/timer.hpp"

// Version information
#define ESP_RAII_LIB_VERSION_MAJOR 0
#define ESP_RAII_LIB_VERSION_MINOR 1
#define ESP_RAII_LIB_VERSION_PATCH 0

namespace esp_raii {
constexpr const char* version = "0.1.0";
}  // namespace esp_raii