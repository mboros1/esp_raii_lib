#include "unity.h"
#include <memory>

#include "esp_raii_lib/gpio.hpp"
#include "esp_raii_lib/timer.hpp"
#include "etl/array.h"
#include "etl/map.h"
#include "etl/optional.h"
#include "etl/string.h"
#include "etl/vector.h"
#include "etl/circular_buffer.h"
#include "etl/bitset.h"

static void test_etl_vector_basic() {
  etl::vector<int, 10> vec;
  TEST_ASSERT_TRUE(vec.empty());
  TEST_ASSERT_EQUAL(0, vec.size());

  vec.push_back(1);
  vec.push_back(2);
  vec.push_back(3);

  TEST_ASSERT_EQUAL(3, vec.size());
  TEST_ASSERT_EQUAL(1, vec[0]);
  TEST_ASSERT_EQUAL(2, vec[1]);
  TEST_ASSERT_EQUAL(3, vec[2]);

  vec.clear();
  TEST_ASSERT_TRUE(vec.empty());
}

static void test_etl_string_basic() {
  etl::string<32> str("Hello");
  TEST_ASSERT_EQUAL_STRING("Hello", str.c_str());
  TEST_ASSERT_EQUAL(5, str.length());

  str += " World";
  TEST_ASSERT_EQUAL_STRING("Hello World", str.c_str());

  etl::string<32> str2;
  str2 = "ESP32";
  TEST_ASSERT_EQUAL_STRING("ESP32", str2.c_str());
}

static void test_etl_array_with_gpio() {
  // Test fixed-size array of GPIO pins
  etl::array<gpio_num_t, 4> pin_numbers = {GPIO_NUM_12, GPIO_NUM_13,
                                           GPIO_NUM_14, GPIO_NUM_15};

  for (const auto& pin : pin_numbers) {
    esp_raii::GPIO gpio(pin);
    TEST_ASSERT_EQUAL(ESP_OK, gpio.configure_output());
    TEST_ASSERT_EQUAL(ESP_OK, gpio.set_level(0));
  }
}

static void test_etl_map_configuration() {
  // Use ETL map for configuration storage
  etl::map<etl::string<16>, int, 5> config;

  config["baud_rate"] = 115200;
  config["timeout_ms"] = 1000;
  config["retry_count"] = 3;

  TEST_ASSERT_EQUAL(3, config.size());
  TEST_ASSERT_EQUAL(115200, config["baud_rate"]);
  TEST_ASSERT_EQUAL(1000, config["timeout_ms"]);

  auto it = config.find("retry_count");
  TEST_ASSERT_TRUE(it != config.end());
  TEST_ASSERT_EQUAL(3, it->second);
}

static void test_etl_optional_with_raii() {
  // Test optional with move-only RAII types
  etl::optional<esp_raii::GPIO> maybe_gpio;

  TEST_ASSERT_FALSE(maybe_gpio.has_value());

  maybe_gpio = esp_raii::GPIO(GPIO_NUM_18);
  TEST_ASSERT_TRUE(maybe_gpio.has_value());

  if (maybe_gpio) {
    TEST_ASSERT_EQUAL(ESP_OK, maybe_gpio->configure_output());
    TEST_ASSERT_EQUAL(ESP_OK, maybe_gpio->set_level(1));
  }

  maybe_gpio.reset();
  TEST_ASSERT_FALSE(maybe_gpio.has_value());
}

struct SensorReading {
  float temperature;
  float humidity;
  uint32_t timestamp;
};

static void test_etl_circular_buffer() {
  // Circular buffer for sensor readings
  etl::circular_buffer<SensorReading, 5> readings;

  TEST_ASSERT_TRUE(readings.empty());

  // Add readings
  for (int i = 0; i < 7; i++) {
    SensorReading reading = {
        .temperature = 20.0f + i, 
        .humidity = 50.0f + i, 
        .timestamp = static_cast<uint32_t>(1000 + i)};
    readings.push(reading);
  }

  // Buffer should contain last 5 readings (indices 2-6)
  TEST_ASSERT_EQUAL(5, readings.size());
  TEST_ASSERT_FALSE(readings.empty());
  TEST_ASSERT_TRUE(readings.full());

  SensorReading front = readings.front();
  TEST_ASSERT_FLOAT_WITHIN(0.1f, 22.0f, front.temperature);
  TEST_ASSERT_EQUAL(1002, front.timestamp);
}

static void test_etl_delegate_vector() {
  // Vector of callbacks
  etl::vector<etl::delegate<void(int)>, 4> callbacks;

  int sum = 0;
  int product = 1;

  auto adder = [&sum](int value) { sum += value; };
  auto multiplier = [&product](int value) { product *= value; };

  callbacks.push_back(adder);
  callbacks.push_back(multiplier);

  // Call all callbacks
  for (auto& callback : callbacks) {
    callback(5);
  }

  TEST_ASSERT_EQUAL(5, sum);
  TEST_ASSERT_EQUAL(5, product);

  // Call again
  for (auto& callback : callbacks) {
    callback(3);
  }

  TEST_ASSERT_EQUAL(8, sum);
  TEST_ASSERT_EQUAL(15, product);
}

static void test_etl_bitset() {
  // Use bitset for GPIO pin tracking
  etl::bitset<40> gpio_in_use;

  TEST_ASSERT_TRUE(gpio_in_use.none());
  TEST_ASSERT_FALSE(gpio_in_use.any());

  // Mark some pins as in use
  gpio_in_use.set(2);
  gpio_in_use.set(4);
  gpio_in_use.set(15);
  gpio_in_use.set(33);

  TEST_ASSERT_TRUE(gpio_in_use.test(2));
  TEST_ASSERT_TRUE(gpio_in_use.test(4));
  TEST_ASSERT_FALSE(gpio_in_use.test(3));
  TEST_ASSERT_EQUAL(4, gpio_in_use.count());

  // Clear a pin
  gpio_in_use.reset(4);
  TEST_ASSERT_FALSE(gpio_in_use.test(4));
  TEST_ASSERT_EQUAL(3, gpio_in_use.count());
}

// Unity test group runner
void run_etl_integration_tests() {
  RUN_TEST(test_etl_vector_basic);
  RUN_TEST(test_etl_string_basic);
  RUN_TEST(test_etl_array_with_gpio);
  RUN_TEST(test_etl_map_configuration);
  RUN_TEST(test_etl_optional_with_raii);
  RUN_TEST(test_etl_circular_buffer);
  RUN_TEST(test_etl_delegate_vector);
  RUN_TEST(test_etl_bitset);
}
