idf_component_register(
    SRCS 
        "Sorter.cpp"            # Jeśli masz oddzielny main.cpp, dopisz go tutaj
    INCLUDE_DIRS 
        "."
    REQUIRES 
        motorControl 
        sensors 
        libs 
        freertos 
        driver
)