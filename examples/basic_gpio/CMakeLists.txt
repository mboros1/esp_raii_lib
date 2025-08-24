# ESP-IDF project with ESP RAII Library
cmake_minimum_required(VERSION 3.16)

# Include ESP RAII Library as a component
set(EXTRA_COMPONENT_DIRS "../../")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(basic_gpio_example)