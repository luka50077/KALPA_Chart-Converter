cmake_minimum_required(VERSION 3.20)
project(frame_viewer)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
add_executable(frame_viewer video.cpp frame_viewer.cpp)
target_link_libraries(frame_viewer ${OpenCV_LIBS})
