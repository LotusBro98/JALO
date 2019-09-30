cmake_minimum_required(VERSION 3.0)

project(JALO)

find_package(OpenCV REQUIRED)
find_package(OpenPose REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(${OpenCV_INCLUDE_DIRS} include 3party/stl-parser 3party/darknet/include)
link_directories(3party/darknet)

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
target_link_libraries(shoulders ${OpenCV_LIBS} ${OpenPose_LIBS} caffe boost_system mysqlcppconn darknet)

add_executable(summary
        src/Camera.cpp
        src/MathUtils.cpp
        src/Config.cpp
        src/Person.cpp
        src/Room.cpp
        3party/stl-parser/stlParser.cpp
        3party/stl-parser/vec/vec.cpp
        src/summary.cpp)
target_link_libraries(summary ${OpenCV_LIBS} ${OpenPose_LIBS} caffe boost_system mysqlcppconn)