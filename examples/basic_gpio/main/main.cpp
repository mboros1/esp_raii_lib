// Copyright 2024 Martin Boros
// SPDX-License-Identifier: Apache-2.0

#include <cstdio>

extern "C" {
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
}

#include "esp_raii_lib/gpio.hpp"
#include "etl/array.h"

static const char* TAG = "GPIO_EXAMPLE";

extern "C" void app_main(void) {
  ESP_LOGI(TAG, "Basic GPIO Example - ESP RAII Library");

  // Example 1: Simple LED control
  {
    esp_raii::GPIO led(GPIO_NUM_2);  // Built-in LED on many ESP32 boards
    
    if (led.configure_output() == ESP_OK) {
      ESP_LOGI(TAG, "LED configured as output");
      
      // Blink LED 5 times
      for (int i = 0; i < 5; i++) {
        led.set_level(1);
        ESP_LOGI(TAG, "LED ON");
        vTaskDelay(pdMS_TO_TICKS(500));
        
        led.set_level(0);
        ESP_LOGI(TAG, "LED OFF");
        vTaskDelay(pdMS_TO_TICKS(500));
      }
    }
  } // LED GPIO automatically cleaned up here

  // Example 2: Input button with pull-up
  {
    esp_raii::GPIO button(GPIO_NUM_0);  // BOOT button on most boards
    
    if (button.configure_input(GPIO_PULLUP_ONLY) == ESP_OK) {
      ESP_LOGI(TAG, "Button configured as input with pull-up");
      
      // Read button state for 10 seconds
      for (int i = 0; i < 100; i++) {
        int level = button.get_level();
        if (level == 0) {  // Button pressed (active low)
          ESP_LOGI(TAG, "Button pressed!");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
      }
    }
  } // Button GPIO automatically cleaned up here

  // Example 3: Multiple GPIO pins with ETL container
  {
    ESP_LOGI(TAG, "Managing multiple GPIO pins with ETL array");
    
    etl::array<esp_raii::GPIO, 3> leds{
      esp_raii::GPIO(GPIO_NUM_12),
      esp_raii::GPIO(GPIO_NUM_13),
      esp_raii::GPIO(GPIO_NUM_14)
    };
    
    // Configure all as outputs
    for (auto& led : leds) {
      led.configure_output();
    }
    
    // Pattern animation
    for (int cycle = 0; cycle < 3; cycle++) {
      for (size_t i = 0; i < leds.size(); i++) {
        leds[i].set_level(1);
        vTaskDelay(pdMS_TO_TICKS(200));
        leds[i].set_level(0);
      }
    }
  } // All GPIO pins automatically cleaned up here

  ESP_LOGI(TAG, "GPIO example completed!");
}