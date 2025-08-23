# ESP RAII Library Test Application

This directory contains Unity-based tests for the ESP RAII Library.

## Test Structure

- `test_gpio.cpp` - Tests for GPIO RAII wrapper
- `test_timer.cpp` - Tests for Timer RAII wrapper  
- `test_etl_integration.cpp` - Tests for ETL container integration
- `test_main.cpp` - Main test runner

## Building and Running Tests

### Local Hardware Testing

```bash
# Configure for your target
idf.py set-target esp32

# Build the test application
idf.py build

# Flash and monitor
idf.py -p /dev/ttyUSB0 flash monitor
```

### QEMU Testing (ESP32)

```bash
# Build for ESP32
idf.py set-target esp32
idf.py build

# Merge binaries for QEMU
idf.py merge_bin -o build/flash_image.bin @build/flash_args

# Run in QEMU
qemu-system-xtensa \
  -nographic \
  -machine esp32 \
  -drive file=build/flash_image.bin,if=mtd,format=raw
```

### QEMU Testing (ESP32-C3)

```bash
# Build for ESP32-C3
idf.py set-target esp32c3
idf.py build

# Merge binaries for QEMU
idf.py merge_bin -o build/flash_image.bin @build/flash_args

# Run in QEMU
qemu-system-riscv32 \
  -nographic \
  -machine esp32c3 \
  -drive file=build/flash_image.bin,if=mtd,format=raw
```

## Test Coverage

### GPIO Tests
- Constructor/destructor
- Output configuration
- Input configuration with pull resistors
- Move semantics
- Multiple pin management
- Integration with ETL containers

### Timer Tests
- Constructor/destructor
- One-shot timer operation
- Periodic timer operation
- Stop and restart functionality
- Move semantics
- Lambda captures
- Member function callbacks
- Error handling

### ETL Integration Tests
- Vector operations
- String operations
- Array with GPIO
- Map for configuration
- Optional with RAII types
- Circular buffer
- Delegate vectors
- Bitset for pin tracking

## CI Integration

Tests are automatically run in GitHub Actions using QEMU for both ESP32 (Xtensa) and ESP32-C3 (RISC-V) targets.

The CI workflow:
1. Builds the test application
2. Merges binaries for QEMU
3. Runs tests in QEMU
4. Checks for "TESTS_COMPLETED: PASS/FAIL" marker
5. Reports results

## Adding New Tests

1. Create a new test file (e.g., `test_new_feature.cpp`)
2. Add it to `main/CMakeLists.txt` SRCS list
3. Create test functions using Unity macros
4. Add a test runner function
5. Call the runner from `test_main.cpp`

Example test structure:
```cpp
#include "unity.h"

static void test_feature_basic()
{
    TEST_ASSERT_EQUAL(expected, actual);
}

void run_feature_tests()
{
    RUN_TEST(test_feature_basic);
}
```

## Debugging Failed Tests

1. Check the Unity output for specific test failures
2. Add `ESP_LOGI()` statements for debugging
3. Run tests on real hardware for more detailed debugging
4. Use GDB with QEMU for step debugging:

```bash
# Terminal 1: Start QEMU with GDB server
qemu-system-xtensa \
  -nographic \
  -machine esp32 \
  -drive file=build/flash_image.bin,if=mtd,format=raw \
  -s -S

# Terminal 2: Connect GDB
xtensa-esp32-elf-gdb build/esp_raii_lib_test.elf
(gdb) target remote :1234
(gdb) continue
```