cmake_minimum_required(VERSION 3.5)
project(sanitizerDemo)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB SRC_FILES *.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
add_executable(${PROJECT_NAME} ${SRC_FILES})