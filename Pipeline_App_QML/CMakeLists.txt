cmake_minimum_required(VERSION 3.14)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick) #
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

set(PROJECT_SOURCES
        main.cpp
        qml.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Pipeline_App_QML
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        graphmodelservice.h
        graphmodelservice.cpp
        actornode.h
        actornode.cpp
        pythonscriptactornode.h
        pythonscriptactornode.cpp
        constants.h
        pythonfileactornode.h
        pythonfileactornode.cpp #
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Pipeline_App_QML APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Pipeline_App_QML
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Pipeline_App_QML
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(Pipeline_App_QML PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(Pipeline_App_QML
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Actor NodeCore NodeGraph ${Python3_LIBRARIES})

target_compile_definitions(Pipeline_App_QML PRIVATE -D_hypot=hypot)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.pipelineapp)
endif()
set_target_properties(Pipeline_App_QML PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Pipeline_App_QML
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(Pipeline_App_QML)
    qt_finalize_executable(Pipeline_App_QML)
endif()
