cmake_minimum_required(VERSION 3.20)

set(vcpkg_path "C:/Developer/Source/vcpkg")

set(CMAKE_TOOLCHAIN_FILE "${vcpkg_path}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(CMAKE_CXX_STANDARD 23)

project(Rasterizer)

include_directories("${vcpkg_path}/installed/x64-windows/include")

file(GLOB SRC_FILES
  "${PROJECT_SOURCE_DIR}/src/*.c"
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/*.h"
  "${PROJECT_SOURCE_DIR}/src/*.hpp")
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

find_package(OpenCV CONFIG REQUIRED)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})
