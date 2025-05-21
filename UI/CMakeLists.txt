cmake_minimum_required(VERSION 3.5)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

add_library(NodeGraph
            Widgets/nodegraphview.h
            Widgets/nodegraphview.cpp
            Models/nodegraphmodel.h
            Models/nodegraphmodel.cpp
            Widgets/GraphicsItems/basenodegraphicsitem.h
            Widgets/GraphicsItems/basenodegraphicsitem.cpp
            Models/nodeitem.h
            Models/nodeitem.cpp
            Widgets/GraphicsItems/connectiongraphicsitem.h
            Widgets/GraphicsItems/connectiongraphicsitem.cpp
            Widgets/GraphicsItems/portgraphicsitem.h
            Widgets/GraphicsItems/portgraphicsitem.cpp
            Widgets/nodegraphicsscene.h
            Widgets/nodegraphicsscene.cpp

)

target_link_libraries(NodeGraph PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(NodeGraph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
