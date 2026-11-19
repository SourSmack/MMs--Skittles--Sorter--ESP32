
if("${IDF_TARGET}" STREQUAL "linux")
    idf_component_register(
        SRCS
            "ScurvePlanner.cpp"
        INCLUDE_DIRS
            "include"
        REQUIRES
            external
    )
else()
    idf_component_register(
        SRCS
            "FastAccelWrapper.cpp"
            "ScurvePlanner.cpp"
        INCLUDE_DIRS
            "include"
        REQUIRES
            external
            gin66__FastAccelStepper
    )
endif()
