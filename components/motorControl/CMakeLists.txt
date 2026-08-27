idf_component_register(
    SRCS 
    
        "ScurvePlanner.cpp"
    INCLUDE_DIRS 
        "include"               # Udostępnia folder include/ na zewnątrz
    REQUIRES 
        includes driver freertos         # Zależności prywatne (potrzebne tylko w .cpp)
)