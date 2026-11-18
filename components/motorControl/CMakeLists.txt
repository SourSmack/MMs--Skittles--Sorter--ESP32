idf_component_register(
    SRCS 
    
        "ScurvePlanner.cpp" "FastAccelWrappper.cpp"
    INCLUDE_DIRS 
        "include"               
    REQUIRES 
    external    gin66__FastAccelStepper 
)
