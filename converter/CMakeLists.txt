cmake_minimum_required(VERSION 3.20)
project(chart_converter)
set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wpedantic")

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)
find_package(OpenCV REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(chart_converter ${SRC_FILES})

target_include_directories(chart_converter PRIVATE ${INC_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(chart_converter PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(chart_converter PRIVATE ${OpenCV_LIBS})
