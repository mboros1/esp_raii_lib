#include "unity.h"
#include "esp_raii_lib/timer.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <atomic>

static std::atomic<int> timer_callback_count{0};
static std::atomic<bool> timer_callback_executed{false};

static void test_timer_constructor_destructor()
{
    // Test that timer can be constructed and destructed
    {
        esp_raii::Timer timer;
        TEST_ASSERT_FALSE(timer.is_active());
    }
    TEST_PASS_MESSAGE("Timer constructor/destructor test passed");
}

static void test_timer_create_and_start()
{
    esp_raii::Timer timer;
    timer_callback_count = 0;
    timer_callback_executed = false;
    
    auto callback = []() {
        timer_callback_count++;
        timer_callback_executed = true;
    };
    
    // Create timer
    esp_err_t result = timer.create("test_timer", callback, false);
    TEST_ASSERT_EQUAL(ESP_OK, result);
    
    // Start one-shot timer
    result = timer.start(100000); // 100ms
    TEST_ASSERT_EQUAL(ESP_OK, result);
    TEST_ASSERT_TRUE(timer.is_active());
    
    // Wait for timer to fire
    vTaskDelay(pdMS_TO_TICKS(150));
    
    TEST_ASSERT_TRUE(timer_callback_executed.load());
    TEST_ASSERT_EQUAL(1, timer_callback_count.load());
    TEST_ASSERT_FALSE(timer.is_active()); // One-shot timer should stop
}

static void test_timer_periodic()
{
    esp_raii::Timer timer;
    timer_callback_count = 0;
    
    auto callback = []() {
        timer_callback_count++;
    };
    
    // Create periodic timer
    esp_err_t result = timer.create("periodic_timer", callback, true);
    TEST_ASSERT_EQUAL(ESP_OK, result);
    
    // Start periodic timer
    result = timer.start(50000); // 50ms period
    TEST_ASSERT_EQUAL(ESP_OK, result);
    
    // Let it run for ~250ms (expect ~5 callbacks)
    vTaskDelay(pdMS_TO_TICKS(260));
    
    result = timer.stop();
    TEST_ASSERT_EQUAL(ESP_OK, result);
    
    int count = timer_callback_count.load();
    TEST_ASSERT_GREATER_OR_EQUAL(4, count);
    TEST_ASSERT_LESS_OR_EQUAL(6, count);
}

static void test_timer_stop_and_restart()
{
    esp_raii::Timer timer;
    timer_callback_count = 0;
    
    auto callback = []() {
        timer_callback_count++;
    };
    
    // Create and start timer
    TEST_ASSERT_EQUAL(ESP_OK, timer.create("restart_timer", callback, true));
    TEST_ASSERT_EQUAL(ESP_OK, timer.start(50000)); // 50ms
    
    vTaskDelay(pdMS_TO_TICKS(120));
    int count1 = timer_callback_count.load();
    TEST_ASSERT_GREATER_OR_EQUAL(2, count1);
    
    // Stop timer
    TEST_ASSERT_EQUAL(ESP_OK, timer.stop());
    vTaskDelay(pdMS_TO_TICKS(100));
    int count2 = timer_callback_count.load();
    
    // Restart timer
    TEST_ASSERT_EQUAL(ESP_OK, timer.restart(50000));
    vTaskDelay(pdMS_TO_TICKS(120));
    int count3 = timer_callback_count.load();
    
    TEST_ASSERT_GREATER_THAN(count2, count3);
    timer.stop();
}

static void test_timer_move_semantics()
{
    timer_callback_count = 0;
    
    auto callback = []() {
        timer_callback_count++;
    };
    
    // Test move constructor
    esp_raii::Timer timer1;
    TEST_ASSERT_EQUAL(ESP_OK, timer1.create("move_timer", callback, false));
    TEST_ASSERT_EQUAL(ESP_OK, timer1.start(100000));
    
    esp_raii::Timer timer2(std::move(timer1));
    TEST_ASSERT_TRUE(timer2.is_active());
    
    vTaskDelay(pdMS_TO_TICKS(150));
    TEST_ASSERT_EQUAL(1, timer_callback_count.load());
    
    // Test move assignment
    esp_raii::Timer timer3;
    timer3 = std::move(timer2);
    
    // Original timers should be empty
    TEST_ASSERT_FALSE(timer1.is_active());
    TEST_ASSERT_FALSE(timer2.is_active());
}

static void test_timer_with_lambda_capture()
{
    esp_raii::Timer timer;
    int local_counter = 0;
    bool flag = false;
    
    auto callback = [&local_counter, &flag]() {
        local_counter++;
        flag = true;
    };
    
    TEST_ASSERT_EQUAL(ESP_OK, timer.create("lambda_timer", callback, false));
    TEST_ASSERT_EQUAL(ESP_OK, timer.start(50000));
    
    vTaskDelay(pdMS_TO_TICKS(100));
    
    TEST_ASSERT_TRUE(flag);
    TEST_ASSERT_EQUAL(1, local_counter);
}

static void test_timer_invalid_operations()
{
    esp_raii::Timer timer;
    
    // Try to start without creating
    TEST_ASSERT_NOT_EQUAL(ESP_OK, timer.start(100000));
    
    // Try to stop without starting
    TEST_ASSERT_NOT_EQUAL(ESP_OK, timer.stop());
    
    // Create timer
    auto dummy_callback = []() {};
    TEST_ASSERT_EQUAL(ESP_OK, timer.create("invalid_ops", dummy_callback, false));
    
    // Try to create again (should fail)
    TEST_ASSERT_NOT_EQUAL(ESP_OK, timer.create("duplicate", dummy_callback, false));
}

class TestClass {
public:
    int count = 0;
    void timer_method() { count++; }
};

static void test_timer_with_member_function()
{
    esp_raii::Timer timer;
    TestClass test_obj;
    
    // Create delegate to member function
    auto callback = etl::delegate<void()>::create<TestClass, &TestClass::timer_method>(test_obj);
    
    TEST_ASSERT_EQUAL(ESP_OK, timer.create("member_timer", callback, false));
    TEST_ASSERT_EQUAL(ESP_OK, timer.start(50000));
    
    vTaskDelay(pdMS_TO_TICKS(100));
    
    TEST_ASSERT_EQUAL(1, test_obj.count);
}

// Unity test group runner
void run_timer_tests()
{
    RUN_TEST(test_timer_constructor_destructor);
    RUN_TEST(test_timer_create_and_start);
    RUN_TEST(test_timer_periodic);
    RUN_TEST(test_timer_stop_and_restart);
    RUN_TEST(test_timer_move_semantics);
    RUN_TEST(test_timer_with_lambda_capture);
    RUN_TEST(test_timer_invalid_operations);
    RUN_TEST(test_timer_with_member_function);
}