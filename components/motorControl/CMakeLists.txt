idf_component_register(
    SRCS 
    
        "ScurvePlanner.cpp" "FastAccelWrappper.cpp"
    INCLUDE_DIRS 
        "include"               # Udostępnia folder include/ na zewnątrz
    REQUIRES 
        libs driver freertos interCommunication     # Zależności prywatne (potrzebne tylko w .cpp)
)