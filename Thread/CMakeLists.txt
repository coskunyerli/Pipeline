cmake_minimum_required(VERSION 3.5)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(Actor
        actor.h
        actor.cpp
        actorrunnable.h
        actorrunnable.cpp
        behaviour.h
        behaviour.cpp
)

target_link_libraries(Actor PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_include_directories(Actor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
