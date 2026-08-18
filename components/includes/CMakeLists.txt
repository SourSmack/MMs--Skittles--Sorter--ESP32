file(GLOB_RECURSE sources "*.cpp" "*.c")

idf_component_register(
    SRCS ${sources}
    INCLUDE_DIRS "." 
                 "FastAccelStepper/src"  # <-- To rozwiązuje problem z brakiem plików
                 "etl/include"
                 "s-curve-motion-planning"
REQUIRES driver esp_driver_gpio esp_timer esp_driver_rmt
)