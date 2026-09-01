idf_component_register(SRCS "Sorter_test.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES my_module freertos )

# Pobieranie Google Test / Google Mock
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# Zapobiegamy instalacji gtest w systemie operacyjnym
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# UWAGA: W ESP-IDF nie linkujemy "gtest_main", ponieważ ESP-IDF 
# dostarcza własną funkcję main() w porcie na Linuxa.
# Linkujemy tylko biblioteki gtest i gmock.
target_link_libraries(${COMPONENT_LIB} PUBLIC gtest gmock)