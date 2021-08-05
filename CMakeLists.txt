cmake_minimum_required(VERSION 3.9)
set(proj_name "SmoothCurve")
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

set(RESOURCE_FILES ${PROJECT_SOURCE_DIR}/resource/qml.qrc)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/app)
include_directories(${PROJECT_SOURCE_DIR}/bspline_library)
include_directories(${PROJECT_SOURCE_DIR}/spline)
include(${PROJECT_SOURCE_DIR}/src.cmake)

add_executable(${proj_name} WIN32 ${SOURCE_FILES} ${RESOURCE_FILES})

target_link_libraries(${proj_name}
    Qt5::Widgets
    Qt5::Qml
    Qt5::Quick
)