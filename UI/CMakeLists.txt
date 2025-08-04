cmake_minimum_required(VERSION 3.5)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

add_library(NodeGraph
            Widgets/nodegraphview.h
            Widgets/nodegraphview.cpp
            Models/basetreemodel.h

            Models/treeitem.h
            Widgets/GraphicsItems/basenodegraphicsitem.h
            Widgets/GraphicsItems/basenodegraphicsitem.cpp

            Widgets/GraphicsItems/connectiongraphicsitem.h
            Widgets/GraphicsItems/connectiongraphicsitem.cpp
            Widgets/GraphicsItems/portgraphicsitem.h
            Widgets/GraphicsItems/portgraphicsitem.cpp
            Widgets/nodegraphicsscene.h
            Widgets/nodegraphicsscene.cpp
            Models/nodegraphtreemodel.h
            Models/nodegraphtreemodel.cpp
            Models/basetreemodel.cpp
            Models/treeitem.cpp
            Models/nodegraphmodel.h
            Models/nodegraphmodel.cpp
            Widgets/GraphicsItems/connectionpreviewgraphicsitem.h
            Widgets/GraphicsItems/connectionpreviewgraphicsitem.cpp
            Models/connectiongraphmodel.h
            Models/connectiongraphmodel.cpp
            Models/nodegraphviewmodel.h
            Models/nodegraphviewmodel.cpp
            Models/connectiongraphviewmodel.h
            Models/connectiongraphviewmodel.cpp
            Models/nodeuimanager.h
            Models/nodeuimanager.cpp
            Models/portgraphviewmodel.h
            Models/portgraphviewmodel.cpp
            Models/constants.h


)


target_link_libraries(NodeGraph PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets NodeCore)
target_include_directories(NodeGraph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
