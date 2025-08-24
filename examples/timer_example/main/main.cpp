// Copyright 2024 Martin Boros
// SPDX-License-Identifier: Apache-2.0

#include <atomic>
#include <cstdio>

extern "C" {
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
}

#include "esp_raii_lib/gpio.hpp"
#include "esp_raii_lib/timer.hpp"

static const char* TAG = "TIMER_EXAMPLE";

// Global counters for demonstration
static std::atomic<int> periodic_counter{0};
static std::atomic<bool> one_shot_triggered{false};

// Timer callback functions
static void periodic_callback() {
  int count = ++periodic_counter;
  ESP_LOGI(TAG, "Periodic timer fired! Count: %d", count);
}

static void one_shot_callback() {
  one_shot_triggered = true;
  ESP_LOGI(TAG, "One-shot timer fired!");
}

extern "C" void app_main(void) {
  ESP_LOGI(TAG, "Timer Example - ESP RAII Library");

  // Example 1: Simple periodic timer
  {
    ESP_LOGI(TAG, "Starting periodic timer (1 second interval)");
    
    esp_raii::Timer periodic_timer;
    
    // Create delegate for the callback
    auto callback_delegate = etl::delegate<void()>::create<periodic_callback>();
    
    if (periodic_timer.create("periodic", callback_delegate, true) == ESP_OK) {
      periodic_timer.start(1000000);  // 1 second in microseconds
      
      // Let it run for 5 seconds
      vTaskDelay(pdMS_TO_TICKS(5500));
      
      periodic_timer.stop();
      ESP_LOGI(TAG, "Periodic timer stopped after %d ticks", 
               periodic_counter.load());
    }
  } // Timer automatically cleaned up here

  // Example 2: One-shot timer
  {
    ESP_LOGI(TAG, "Starting one-shot timer (2 second delay)");
    
    esp_raii::Timer one_shot_timer;
    
    // Create delegate for the callback
    auto callback_delegate = etl::delegate<void()>::create<one_shot_callback>();
    
    if (one_shot_timer.create("oneshot", callback_delegate, false) == ESP_OK) {
      one_shot_timer.start(2000000);  // 2 seconds in microseconds
      
      // Wait for timer to fire
      vTaskDelay(pdMS_TO_TICKS(2500));
      
      ESP_LOGI(TAG, "One-shot timer completed: %s", 
               one_shot_triggered ? "FIRED" : "NOT FIRED");
    }
  } // Timer automatically cleaned up here

  // Example 3: Timer with LED blinking using lambda
  {
    ESP_LOGI(TAG, "Timer-controlled LED blinking");
    
    static esp_raii::GPIO led(GPIO_NUM_2);
    led.configure_output();
    
    // Use a static variable to maintain state between callbacks
    static bool led_state = false;
    
    esp_raii::Timer blink_timer;
    
    // Lambda callback
    auto blink_lambda = []() {
      led_state = !led_state;
      led.set_level(led_state ? 1 : 0);
      ESP_LOGD(TAG, "LED %s", led_state ? "ON" : "OFF");
    };
    
    // Create delegate from lambda
    auto blink_delegate = etl::delegate<void()>::create(blink_lambda);
    
    if (blink_timer.create("blink", blink_delegate, true) == ESP_OK) {
      blink_timer.start(250000);  // 250ms in microseconds
      
      // Blink for 5 seconds
      vTaskDelay(pdMS_TO_TICKS(5000));
      
      blink_timer.stop();
      led.set_level(0);  // Ensure LED is off
    }
  } // Timer automatically cleaned up here

  // Example 4: Multiple timers with context in class
  {
    ESP_LOGI(TAG, "Multiple timers with different intervals");
    
    struct TimerContext {
      const char* name;
      int count;
      
      void callback() {
        count++;
        ESP_LOGI(TAG, "%s timer: tick %d", name, count);
      }
    };
    
    static TimerContext fast_ctx = {"FAST", 0};
    static TimerContext slow_ctx = {"SLOW", 0};
    
    esp_raii::Timer fast_timer;
    esp_raii::Timer slow_timer;
    
    // Create member function delegates
    auto fast_delegate = etl::delegate<void()>::create<TimerContext, &TimerContext::callback>(fast_ctx);
    auto slow_delegate = etl::delegate<void()>::create<TimerContext, &TimerContext::callback>(slow_ctx);
    
    fast_timer.create("fast", fast_delegate, true);
    slow_timer.create("slow", slow_delegate, true);
    
    fast_timer.start(500000);   // 500ms in microseconds
    slow_timer.start(1500000);  // 1.5s in microseconds
    
    // Run for 6 seconds
    vTaskDelay(pdMS_TO_TICKS(6000));
    
    fast_timer.stop();
    slow_timer.stop();
    
    ESP_LOGI(TAG, "Final counts - Fast: %d, Slow: %d", 
             fast_ctx.count, slow_ctx.count);
  } // All timers automatically cleaned up here

  // Example 5: Timer with restart
  {
    ESP_LOGI(TAG, "Timer restart example");
    
    static int restart_count = 0;
    esp_raii::Timer restart_timer;
    
    auto restart_callback = []() {
      restart_count++;
      ESP_LOGI(TAG, "Timer tick: %d", restart_count);
    };
    
    restart_timer.create("restart", etl::delegate<void()>::create(restart_callback), true);
    
    // Start with 1 second interval
    ESP_LOGI(TAG, "Starting with 1 second interval");
    restart_timer.start(1000000);
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    // Restart with 500ms interval
    ESP_LOGI(TAG, "Restarting with 500ms interval");
    restart_timer.stop();
    restart_count = 0;
    restart_timer.start(500000);
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    restart_timer.stop();
    ESP_LOGI(TAG, "Final restart count: %d", restart_count);
  }

  ESP_LOGI(TAG, "Timer example completed!");
}