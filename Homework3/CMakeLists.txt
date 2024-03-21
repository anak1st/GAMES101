cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Rasterizer)

file(GLOB SRC_FILES
  "${PROJECT_SOURCE_DIR}/src/*.c"
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  "${PROJECT_SOURCE_DIR}/src/*.h"
  "${PROJECT_SOURCE_DIR}/src/*.hpp")
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

find_package(Eigen3 CONFIG REQUIRED)

find_package(OpenCV CONFIG REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${OpenCV_LIBRARIES})

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE fmt::fmt)

# copy compile_commands.json to build directory
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${CMAKE_BINARY_DIR}/build/compile_commands.json
  ${CMAKE_SOURCE_DIR}/build/compile_commands.json)