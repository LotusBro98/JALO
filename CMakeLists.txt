cmake_minimum_required(VERSION 3.0)

project(JALO)

find_package(OpenCV REQUIRED)
find_package(OpenPose REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(shoulders src/shoulders.cpp src/shoulders.cpp)
target_link_libraries(shoulders ${OpenCV_LIBS} ${OpenPose_LIBS} caffe boost_system)