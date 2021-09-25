set (CMAKE_INSTALL_PREFIX /usr/local)

cmake_minimum_required(VERSION 3.0.0)
project(csv2vtt VERSION 0.1.0)
include_directories(include)
set(CMAKE_CXX_STANDARD 17)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        703bd9caab50b139428cea1aaff9974ebee5742e # release-1.10.0
)
FetchContent_MakeAvailable(gtest)

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

set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ
    OWNER_WRITE
    OWNER_EXECUTE
    GROUP_READ
)

install(TARGETS ${This}
        CONFIGURATIONS Release)
