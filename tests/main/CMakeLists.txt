idf_component_register(
    SRCS  "start_tests.cpp" "Sorter_test.cpp" "Nema_test2.cpp" "ScurvePlanner_test.cpp" "FREERTOSWrapper_test.cpp"

         #"Nema_test.cpp"
         "../../main/Sorter.hpp"
    INCLUDE_DIRS "." 
                 "../../main"
                "../../components/motorControl/include/"
    REQUIRES freertos interCommunication libs 
)
include(FetchContent)
FetchContent_Declare(
  magic_enum 
  GIT_REPOSITORY https://github.com/Neargye/magic_enum
  GIT_TAG        v0.9.5 # Stabilna wersja
)

FetchContent_MakeAvailable(magic_enum)

# 2. Pobieranie GTest / GMock
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0 # Stabilna wersja
)

# Wyłączamy generowanie testów samego frameworka GTest
set(gtest_build_tests OFF CACHE BOOL "" FORCE)
set(gmock_build_tests OFF CACHE BOOL "" FORCE)

# TĄ LINIJKĘ DODAJEMY - blokuje próby eksportu GTest z zależnościami ESP-IDF
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)


# 3. Linkowanie pobranego GMock do naszego komponentu testowego
target_link_libraries(   ${COMPONENT_LIB}  PRIVATE gtest gmock magic_enum::magic_enum)
target_link_options(${COMPONENT_LIB} INTERFACE "-Wl,--whole-archive" "$<TARGET_FILE:${COMPONENT_LIB}>" "-Wl,--no-whole-archive"
    "-Wl,--wrap=xTaskCreatePinnedToCore"
    "-Wl,--wrap=vTaskDelay"
    "-Wl,--wrap=xTaskGenericNotify"
    "-Wl,--wrap=xTaskGenericNotifyWait"
    "-Wl,--wrap=vPortYield"
)

