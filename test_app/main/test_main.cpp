#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "unity.h"

static const char* TAG = "RAII_TEST";

// Declare test runners
extern void run_gpio_tests();
extern void run_timer_tests();
extern void run_etl_integration_tests();

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Starting ESP RAII Library Unity Tests");

    // Small delay to ensure proper initialization
    vTaskDelay(pdMS_TO_TICKS(100));

    // Initialize Unity
    UNITY_BEGIN();

    ESP_LOGI(TAG, "Running GPIO tests...");
    run_gpio_tests();

    ESP_LOGI(TAG, "Running Timer tests...");
    run_timer_tests();

    ESP_LOGI(TAG, "Running ETL integration tests...");
    run_etl_integration_tests();

    // Finish Unity
    int failures = UNITY_END();

    if (failures == 0) {
        ESP_LOGI(TAG, "All tests passed!");
    } else {
        ESP_LOGE(TAG, "%d test(s) failed", failures);
    }

    // For QEMU, print special marker for CI to detect completion
    printf("TESTS_COMPLETED: %s\n", failures == 0 ? "PASS" : "FAIL");

    // Keep the task alive for QEMU
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}