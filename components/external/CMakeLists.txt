idf_component_register(
    REQUIRES freertos  esp_driver_gpio
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
    SOURCE_SUBDIR  skip_cmake
)


FetchContent_Declare(
    scurve
    GIT_REPOSITORY https://github.com/grotius-cnc/s-curve-motion-planning.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)

FetchContent_Declare( 
    AGUA 
    GIT_REPOSITORY  https://github.com/SourSmack/AGUA-rt.git
    GIT_TAG main
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(InlineOS   scurve AGUA )


add_library( etl_lib INTERFACE )
target_include_directories( etl_lib INTERFACE "${etl_SOURCE_DIR}/include" )

add_library(scurve_lib STATIC
    "${scurve_SOURCE_DIR}/scurve.cpp"
)
target_include_directories(scurve_lib PUBLIC
    "${scurve_SOURCE_DIR}"
)


target_link_libraries( inlineos_esp_freertos_deps INTERFACE idf::freertos ) 

target_link_libraries(${COMPONENT_LIB} INTERFACE
    InlineOS::esp::freertos
    etl_lib
    scurve_lib
    AGUA::SynchEsp
    idf::esp_driver_gpio
    idf::freertos 
)
