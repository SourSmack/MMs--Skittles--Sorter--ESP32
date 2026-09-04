idf_component_register(
    SRCS "Sorter_test.cpp" 
         #"Nema_test.cpp"
         "../../main/Sorter.hpp"
    INCLUDE_DIRS "." 
                 "../../main"
    REQUIRES freertos interCommunication libs
)

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
target_link_libraries(${COMPONENT_LIB} PRIVATE gtest gmock)