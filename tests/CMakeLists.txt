cmake_minimum_required(VERSION 3.22)
set(EXTRA_COMPONENT_DIRS "../components" "mocks")

set(COMPONENTS
    esp_linux_stubs
    
    external
    main
    sensors
    motorControl
    
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(tests)


