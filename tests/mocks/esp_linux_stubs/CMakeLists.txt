idf_component_register(
    SRCS "src/gpio_stub.cpp" "src/uart_stub.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_common freertos esp_hw_support
)
