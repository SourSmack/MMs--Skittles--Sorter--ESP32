cmake_minimum_required(VERSION 3.22)
set(EXTRA_COMPONENT_DIRS "../components")

# Budujemy tylko główny test oraz bezpieczny interCommunication
set(COMPONENTS main interCommunication)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(tests)