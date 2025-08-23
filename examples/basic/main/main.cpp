#include <stdio.h>
#include "esp_log.h"
#include "esp_raii_lib/esp_raii_lib.hpp"
#include "etl/vector.h"
#include "etl/string.h"

static const char* TAG = "RAII_EXAMPLE";

// Example class using RAII GPIO
class LedController {
public:
    explicit LedController(gpio_num_t pin) : gpio_(pin) {
        if (gpio_.configure_output() == ESP_OK) {
            ESP_LOGI(TAG, "LED GPIO %d configured", pin);
        }
    }
    
    void toggle() {
        state_ = !state_;
        gpio_.set_level(state_ ? 1 : 0);
    }
    
private:
    esp_raii::GPIO gpio_;
    bool state_ = false;
};

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "ESP RAII Library Example");
    
    // Example 1: Using RAII GPIO wrapper
    {
        LedController led(GPIO_NUM_2);
        
        for (int i = 0; i < 5; i++) {
            led.toggle();
            ESP_LOGI(TAG, "LED toggled");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    } // GPIO automatically cleaned up here
    
    // Example 2: Using RAII Timer with ETL delegate
    {
        esp_raii::Timer timer;
        int counter = 0;
        
        auto timer_callback = [&counter, TAG]() {
            ESP_LOGI(TAG, "Timer fired! Count: %d", ++counter);
        };
        
        if (timer.create("example_timer", timer_callback, true) == ESP_OK) {
            timer.start(1000000); // 1 second
            
            vTaskDelay(pdMS_TO_TICKS(5000)); // Let it run for 5 seconds
            
            timer.stop();
            ESP_LOGI(TAG, "Timer stopped after %d ticks", counter);
        }
    } // Timer automatically cleaned up here
    
    // Example 3: Using ETL containers
    {
        etl::vector<int, 10> measurements;
        measurements.push_back(100);
        measurements.push_back(200);
        measurements.push_back(300);
        
        ESP_LOGI(TAG, "Measurements collected: %zu", measurements.size());
        
        for (const auto& value : measurements) {
            ESP_LOGI(TAG, "  Value: %d", value);
        }
        
        etl::string<64> status_message("System running with RAII and ETL");
        ESP_LOGI(TAG, "Status: %s", status_message.c_str());
    }
    
    ESP_LOGI(TAG, "Example complete - all resources automatically cleaned up!");
}