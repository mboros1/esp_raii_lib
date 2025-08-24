// Copyright 2024 Martin Boros
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <utility>

extern "C" {
#include "unity.h"
}

#include "etl/vector.h"

#include "esp_raii_lib/gpio.hpp"

// Test fixture
static void test_gpio_constructor_destructor() {
  // Test that GPIO can be constructed and destructed without issues
  {
    esp_raii::GPIO gpio(GPIO_NUM_4);
    TEST_ASSERT_EQUAL(GPIO_NUM_4, gpio.pin());
  }
  // GPIO should be automatically reset after destruction
}

static void test_gpio_output_configuration() {
  esp_raii::GPIO gpio(GPIO_NUM_5);

  // Configure as output
  esp_err_t result = gpio.configure_output();
  TEST_ASSERT_EQUAL(ESP_OK, result);

  // Test setting levels
  TEST_ASSERT_EQUAL(ESP_OK, gpio.set_level(1));
  TEST_ASSERT_EQUAL(ESP_OK, gpio.set_level(0));
}

static void test_gpio_input_configuration() {
  esp_raii::GPIO gpio(GPIO_NUM_18);

  // Configure as input with pull-up
  esp_err_t result = gpio.configure_input(GPIO_PULLUP_ONLY);
  TEST_ASSERT_EQUAL(ESP_OK, result);

  // Reading input (value depends on hardware state)
  int level = gpio.get_level();
  TEST_ASSERT_TRUE(level == 0 || level == 1);
}

static void test_gpio_move_semantics() {
  // Test move constructor
  esp_raii::GPIO gpio1(GPIO_NUM_19);
  gpio1.configure_output();

  esp_raii::GPIO gpio2(std::move(gpio1));
  TEST_ASSERT_EQUAL(GPIO_NUM_19, gpio2.pin());

  // Test move assignment
  esp_raii::GPIO gpio3(GPIO_NUM_21);
  gpio3 = std::move(gpio2);
  TEST_ASSERT_EQUAL(GPIO_NUM_19, gpio3.pin());
}

static void test_gpio_multiple_pins() {
  // Test managing multiple GPIO pins simultaneously
  esp_raii::GPIO led1(GPIO_NUM_12);
  esp_raii::GPIO led2(GPIO_NUM_13);
  esp_raii::GPIO button(GPIO_NUM_14);

  TEST_ASSERT_EQUAL(ESP_OK, led1.configure_output());
  TEST_ASSERT_EQUAL(ESP_OK, led2.configure_output());
  TEST_ASSERT_EQUAL(ESP_OK, button.configure_input(GPIO_PULLUP_ONLY));

  // Toggle LEDs
  TEST_ASSERT_EQUAL(ESP_OK, led1.set_level(1));
  TEST_ASSERT_EQUAL(ESP_OK, led2.set_level(0));
  TEST_ASSERT_EQUAL(ESP_OK, led1.set_level(0));
  TEST_ASSERT_EQUAL(ESP_OK, led2.set_level(1));
}

static void test_gpio_raii_in_vector() {
  // Test RAII behavior with ETL container

  {
    etl::vector<esp_raii::GPIO, 3> gpio_pins;

    // Note: Can't use emplace_back directly due to move-only type
    // Must use move construction
    gpio_pins.push_back(esp_raii::GPIO(GPIO_NUM_15));
    gpio_pins.push_back(esp_raii::GPIO(GPIO_NUM_16));
    gpio_pins.push_back(esp_raii::GPIO(GPIO_NUM_17));

    for (auto& gpio : gpio_pins) {
      TEST_ASSERT_EQUAL(ESP_OK, gpio.configure_output());
      TEST_ASSERT_EQUAL(ESP_OK, gpio.set_level(1));
    }
  }
  // All GPIOs should be automatically cleaned up
}

// Unity test group runner
void run_gpio_tests() {
  RUN_TEST(test_gpio_constructor_destructor);
  RUN_TEST(test_gpio_output_configuration);
  RUN_TEST(test_gpio_input_configuration);
  RUN_TEST(test_gpio_move_semantics);
  RUN_TEST(test_gpio_multiple_pins);
  RUN_TEST(test_gpio_raii_in_vector);
}
