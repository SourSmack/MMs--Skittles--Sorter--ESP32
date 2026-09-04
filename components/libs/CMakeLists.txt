if(IDF_TARGET STREQUAL "linux")
    # ---------------------------------------------------------
    # WERSJA DLA LINUXA (Środowisko testowe GMock)
    # ---------------------------------------------------------
    # Rejestrujemy komponent jako czysto nagłówkowy (header-only).
    # Nie ładujemy żadnych plików .cpp ze sterownikami silników
    # ani nie żądamy systemowego komponentu 'driver'.
    idf_component_register(
        INCLUDE_DIRS "."
                     "etl/include"
    )

else()
    # ---------------------------------------------------------
    # WERSJA DLA ESP32 (Środowisko produkcyjne)
    # ---------------------------------------------------------
    # Pełna kompilacja ze sprzętem, silnikami i sterownikami.
    file(GLOB_RECURSE sources "*.cpp" "*.c")

    idf_component_register(
        SRCS ${sources}
        INCLUDE_DIRS "." 
                     "FastAccelStepper/src" 
                     "etl/include"
                     "s-curve-motion-planning"
        REQUIRES driver esp_driver_gpio esp_timer esp_driver_rmt
    )
endif()