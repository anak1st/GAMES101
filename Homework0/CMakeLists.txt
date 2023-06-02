cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE "~/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
message(STATUS "Vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

project(Test)

include_directories("~/vcpkg/installed/x64-windows/include")

file(GLOB SRC_FILES
  "${PROJECT_SOURCE_DIR}/src/*.c"
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/*.h"
  "${PROJECT_SOURCE_DIR}/src/*.hpp")
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

find_package(OpenCV CONFIG REQUIRED)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})
