cmake_minimum_required(VERSION 3.0)

project(JALO)

find_package(OpenCV REQUIRED)
find_package(OpenPose REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} include 3party/stl-parser)

add_executable(shoulders
        src/Camera.cpp
        src/MathUtils.cpp
        src/Config.cpp
        src/main.cpp
        src/Person.cpp
        src/Room.cpp
        3party/stl-parser/stlParser.cpp
        3party/stl-parser/vec/vec.cpp
        )
target_link_libraries(shoulders ${OpenCV_LIBS} ${OpenPose_LIBS} caffe boost_system)