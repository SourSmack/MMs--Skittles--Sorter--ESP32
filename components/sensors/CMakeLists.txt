idf_component_register(
    SRCS "EventGroupIsrTransoptor.cpp" "Camera.cpp" 
    INCLUDE_DIRS "include"
    REQUIRES libs driver esp_driver_uart freertos
)