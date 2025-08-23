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
}