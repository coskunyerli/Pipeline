cmake_minimum_required(VERSION 3.5)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(NodeCore
    node.h
    node.cpp
    port.h
    port.cpp
    connection.h
    connection.cpp
)

target_link_libraries(NodeCore PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_include_directories(NodeCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
