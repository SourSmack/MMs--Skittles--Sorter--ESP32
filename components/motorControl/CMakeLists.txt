idf_component_register(
    SRCS 
    
        "ScurvePlanner.cpp"
    INCLUDE_DIRS 
        "include"               # Udostępnia folder include/ na zewnątrz
    REQUIRES 
        includes                # Zależność publiczna (widoczna w Twoich plikach .hpp)
    PRIV_REQUIRES 
        driver freertos         # Zależności prywatne (potrzebne tylko w .cpp)
)