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

#include "etl/delegate.h"

#include <utility>

#include "esp_timer.h"

namespace esp_raii {

class Timer {
   public:
    using callback_t = etl::delegate<void()>;

    Timer() : handle_(nullptr) {}

    ~Timer() { destroy(); }

    // Delete copy constructor and copy assignment
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    // Enable move constructor and move assignment
    Timer(Timer&& other) noexcept : handle_(other.handle_), callback_(std::move(other.callback_)) {
        other.handle_ = nullptr;
    }

    Timer& operator=(Timer&& other) noexcept {
        if (this != &other) {
            destroy();
            handle_ = other.handle_;
            callback_ = std::move(other.callback_);
            other.handle_ = nullptr;
        }
        return *this;
    }

    esp_err_t create(const char* name, callback_t callback, bool auto_reload = false) {
        if (handle_) {
            return ESP_ERR_INVALID_STATE;
        }

        callback_ = callback;

        esp_timer_create_args_t args = {.callback = &Timer::timer_callback,
                                        .arg = this,
                                        .dispatch_method = ESP_TIMER_TASK,
                                        .name = name,
                                        .skip_unhandled_events = false};

        esp_err_t ret = esp_timer_create(&args, &handle_);
        if (ret == ESP_OK) {
            auto_reload_ = auto_reload;
        }
        return ret;
    }

    esp_err_t start(uint64_t period_us) {
        if (!handle_) {
            return ESP_ERR_INVALID_STATE;
        }

        if (auto_reload_) {
            return esp_timer_start_periodic(handle_, period_us);
        } else {
            return esp_timer_start_once(handle_, period_us);
        }
    }

    esp_err_t stop() {
        if (!handle_) {
            return ESP_ERR_INVALID_STATE;
        }
        return esp_timer_stop(handle_);
    }

    esp_err_t restart(uint64_t period_us) {
        esp_err_t ret = stop();
        if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
            return ret;
        }
        return start(period_us);
    }

    bool is_active() const { return handle_ && esp_timer_is_active(handle_); }

   private:
    void destroy() {
        if (handle_) {
            esp_timer_stop(handle_);
            esp_timer_delete(handle_);
            handle_ = nullptr;
        }
    }

    static void timer_callback(void* arg) {
        Timer* timer = static_cast<Timer*>(arg);
        if (timer->callback_) {
            timer->callback_();
        }
    }

    esp_timer_handle_t handle_;
    callback_t callback_;
    bool auto_reload_ = false;
};

}  // namespace esp_raii