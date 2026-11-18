idf_component_register(
    REQUIRES freertos esp_timer driver esp_driver_uart
)

include(FetchContent)

FetchContent_Declare(
    InlineOS
    GIT_REPOSITORY https://github.com/SourSmack/InlineOS.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    etl
    GIT_REPOSITORY https://github.com/ETLCPP/etl.git
    GIT_TAG        20.38.14  # lub wybrany stabilny tag
    GIT_SHALLOW    TRUE
)


FetchContent_Declare(
    scurve
    GIT_REPOSITORY https://github.com/grotius-cnc/s-curve-motion-planning.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(InlineOS etl  scurve)

add_library(scurve_lib STATIC
    "${scurve_SOURCE_DIR}/scurve.cpp"
)
target_include_directories(scurve_lib PUBLIC
    "${scurve_SOURCE_DIR}"
)


target_link_libraries( inlineos_esp_freertos_deps INTERFACE idf::freertos ) 

target_link_libraries(${COMPONENT_LIB} PUBLIC
    InlineOS::esp::freertos
    etl::etl
    scurve_lib
)
