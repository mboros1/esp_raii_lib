#pragma once

#include "driver/gpio.h"
#include <utility>

namespace esp_raii {

class GPIO {
public:
    explicit GPIO(gpio_num_t pin) 
        : pin_(pin), configured_(false) {
    }
    
    ~GPIO() {
        if (configured_) {
            gpio_reset_pin(pin_);
        }
    }
    
    // Delete copy constructor and copy assignment
    GPIO(const GPIO&) = delete;
    GPIO& operator=(const GPIO&) = delete;
    
    // Enable move constructor and move assignment
    GPIO(GPIO&& other) noexcept 
        : pin_(other.pin_), configured_(other.configured_) {
        other.configured_ = false;
    }
    
    GPIO& operator=(GPIO&& other) noexcept {
        if (this != &other) {
            if (configured_) {
                gpio_reset_pin(pin_);
            }
            pin_ = other.pin_;
            configured_ = other.configured_;
            other.configured_ = false;
        }
        return *this;
    }
    
    esp_err_t configure_output() {
        esp_err_t ret = gpio_set_direction(pin_, GPIO_MODE_OUTPUT);
        if (ret == ESP_OK) {
            configured_ = true;
        }
        return ret;
    }
    
    esp_err_t configure_input(gpio_pull_mode_t pull_mode = GPIO_FLOATING) {
        esp_err_t ret = gpio_set_direction(pin_, GPIO_MODE_INPUT);
        if (ret == ESP_OK) {
            ret = gpio_set_pull_mode(pin_, pull_mode);
            if (ret == ESP_OK) {
                configured_ = true;
            }
        }
        return ret;
    }
    
    esp_err_t set_level(uint32_t level) {
        return gpio_set_level(pin_, level);
    }
    
    int get_level() {
        return gpio_get_level(pin_);
    }
    
    gpio_num_t pin() const { return pin_; }
    
private:
    gpio_num_t pin_;
    bool configured_;
};

} // namespace esp_raii