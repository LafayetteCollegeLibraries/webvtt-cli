cmake_minimum_required(VERSION 3.0.0)
project(csv2vtt VERSION 0.1.0)
include_directories(include)
set(CMAKE_CXX_STANDARD 17)

enable_testing()

add_subdirectory(gtest)

set (This csv2vtt)

set (Sources
    src/csvrow.cpp
    src/vtterror.cpp
)

add_executable(${This} main.cpp ${Sources})
add_library(csvttlib STATIC ${Sources})

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
