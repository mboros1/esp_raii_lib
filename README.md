# ESP RAII Library

A modern C++ RAII (Resource Acquisition Is Initialization) wrapper library for ESP-IDF, featuring ETL (Embedded Template Library) containers for embedded systems.

## Features

- **RAII Wrappers**: Automatic resource management for ESP-IDF components
- **ETL Integration**: STL-like containers optimized for embedded systems (no dynamic allocation)
- **Header-only**: Easy to integrate, no compilation required for the library itself
- **Move Semantics**: Efficient resource transfer with C++11 move semantics
- **Type Safety**: Strong typing prevents common errors

## Current RAII Wrappers

- `GPIO`: Automatic GPIO pin cleanup
- `Timer`: ESP timer with automatic cleanup and ETL delegate callbacks
- More wrappers coming soon (SPI, I2C, UART, etc.)

## Installation

### Method 1: Clone into Components Directory

```bash
cd your-project/components
git clone https://github.com/yourusername/esp_raii_lib.git
```

### Method 2: As a Git Submodule

```bash
cd your-project
git submodule add https://github.com/yourusername/esp_raii_lib.git components/esp_raii_lib
```

### Method 3: IDF Component Registry (Future)

Once published to the ESP-IDF Component Registry:

```bash
idf.py add-dependency "esp_raii_lib"
```

## Project Structure

```
esp_raii_lib/
├── CMakeLists.txt           # Component configuration
├── idf_component.yml        # IDF Component Registry manifest
├── include/                 # Public headers
│   └── esp_raii_lib/
│       ├── esp_raii_lib.hpp  # Main include file
│       ├── gpio.hpp         # GPIO RAII wrapper
│       └── timer.hpp        # Timer RAII wrapper
├── deps/
│   └── etl/                # ETL headers (vendored)
├── examples/
│   └── basic/              # Basic usage example
└── README.md
```

## Usage

### In your project's CMakeLists.txt

No special configuration needed if the component is in your `components` directory.

For custom component paths:

```cmake
set(EXTRA_COMPONENT_DIRS "path/to/esp_raii_lib")
```

### In your component's CMakeLists.txt

```cmake
idf_component_register(
    SRCS "your_source.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_raii_lib
)
```

### In your code

```cpp
#include "esp_raii_lib/esp_raii_lib.hpp"
#include "etl/vector.h"

void example() {
    // GPIO automatically cleaned up when out of scope
    esp_raii::GPIO led(GPIO_NUM_2);
    led.configure_output();
    led.set_level(1);
    
    // Use ETL containers (no heap allocation)
    etl::vector<int, 10> data;
    data.push_back(42);
}
```

## Example Project

See the `examples/basic` directory for a complete working example:

```bash
cd examples/basic
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Requirements

- ESP-IDF 5.0 or later
- C++17 compiler support (included with ESP-IDF)
- ETL library (included as submodule)

## VS Code Setup

The library includes VS Code configuration files in the main example. Copy the `.vscode` directory to your project and adjust paths as needed.

## Contributing

Contributions are welcome! Please feel free to submit pull requests with new RAII wrappers or improvements.

## Future Additions

- [ ] SPI Master/Slave RAII wrapper
- [ ] I2C Master/Slave RAII wrapper  
- [ ] UART RAII wrapper
- [ ] ADC RAII wrapper
- [ ] PWM/LEDC RAII wrapper
- [ ] Task/Mutex/Semaphore RAII wrappers
- [ ] NVS (Non-Volatile Storage) RAII wrapper

## License

[Your chosen license]

## Acknowledgments

- [ETL (Embedded Template Library)](https://www.etlcpp.com/) for providing STL-like containers for embedded systems