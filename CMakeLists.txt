# ESP RAII Library Component CMakeLists.txt
# This file defines the component for use in other ESP-IDF projects

# Define source files for the component
set(srcs "")

# If there are source files in src directory, add them here
file(GLOB_RECURSE srcs_glob "src/*.cpp" "src/*.c")
if(srcs_glob)
    list(APPEND srcs ${srcs_glob})
endif()

# Register the component
idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include" "deps"
    PRIV_INCLUDE_DIRS "src"
)

# Add compile definitions for ETL
target_compile_definitions(${COMPONENT_LIB} PUBLIC
    ETL_NO_STL=1
    ETL_NO_CHECKS=1
)

# Set C++ standard
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)