cmake_minimum_required(VERSION 3.9)
set(PROJECT "SmoothCurve")
project(SmoothCurve)

string(REGEX REPLACE "/code$" "" PROJECT_OUTPUT_DIR ${PROJECT_SOURCE_DIR})
set(PROJECT_OUTPUT_DIR "${PROJECT_OUTPUT_DIR}/output")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_OUTPUT_DIR}")
set(LIBRARY_OUTPUT_PATH "${PROJECT_OUTPUT_DIR}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Widgets Gui Qml Quick REQUIRED)

add_executable(${PROJECT}
main.cpp
qml.qrc
random_points_data.h
random_points_data.cpp
random_points_view.h
random_points_view.cpp
smooth_curve.h
smooth_curve.cpp
cubic_interp_wrapper.h
cubic_interp_wrapper.cpp
spline.h
)

target_link_libraries(${PROJECT}
    Qt5::Widgets
    Qt5::Qml
    Qt5::Quick
)