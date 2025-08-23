idf_component_register(
    SRCS 
        "test_main.cpp"
        "test_gpio.cpp"
        "test_timer.cpp"
        "test_etl_integration.cpp"
    INCLUDE_DIRS "."
    REQUIRES 
        esp_raii_lib
        unity
)