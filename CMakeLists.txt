cmake_minimum_required(VERSION 3.5)
#
project(Pipeline VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Thread)
add_subdirectory(UI)
add_subdirectory(Pipeline_App)
add_subdirectory(NodeCore)
