# ESP RAII Library Component CMakeLists.txt
# This file defines the component for use in other ESP-IDF projects

# Define source files for the component
set(srcs "")
set(priv_include_dirs "")

# If there are source files in src directory, add them here
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src")
    file(GLOB_RECURSE srcs_glob "src/*.cpp" "src/*.c")
    if(srcs_glob)
        list(APPEND srcs ${srcs_glob})
        list(APPEND priv_include_dirs "src")
    endif()
endif()

# For a header-only library, we need at least one source file for ESP-IDF
# Create a dummy source file if none exist
if(NOT srcs)
    set(dummy_src "${CMAKE_CURRENT_BINARY_DIR}/esp_raii_lib_dummy.c")
    file(WRITE ${dummy_src} "// Dummy source file for header-only library\nvoid esp_raii_lib_dummy(void) {}\n")
    set(srcs ${dummy_src})
endif()

# Register the component
idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include" "deps"
    PRIV_INCLUDE_DIRS ${priv_include_dirs}
)

# Add compile definitions for ETL - Zero cost configuration
target_compile_definitions(${COMPONENT_LIB} PUBLIC
    ETL_NO_STL=1           # No STL dependency
    ETL_NO_CHECKS=1        # Disable runtime checks for performance
    ETL_NO_PROFILE_STRINGS=1  # Disable profile strings to save space
    ETL_THROW_EXCEPTIONS=0    # Never throw exceptions
    ETL_LOG_ERRORS=0          # No error logging overhead
    ETL_IN_UNIT_TEST=0        # Not in unit test mode
    ETL_USE_TYPE_TRAITS_BUILTINS=1  # Use compiler builtins for type traits
    ETL_NO_LARGE_CHAR_SUPPORT=1     # No need for large char support
)

# Set C++ standard
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)