# Example project CMakeLists
cmake_minimum_required(VERSION 3.16)

# Add the parent directory as a component path
set(EXTRA_COMPONENT_DIRS "../..")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp_raii_example)