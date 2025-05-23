cmake_minimum_required(VERSION 3.17)

project(skulpture)

option(USE_QT6 "Use Qt6 instead of Qt5" ON)
option(USE_GUI_CONFIG "Build GUI configuration tool" ON)

if (USE_QT6)
    set(QT_MIN_VERSION "6.2.0")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
    set(QT_MIN_VERSION "5.15.2")
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})


if (USE_QT6)
    find_package(QT NAMES Qt6 COMPONENTS Core Gui Widgets REQUIRED)
else()
    find_package(QT NAMES Qt5 COMPONENTS Core Gui Widgets REQUIRED)
endif()

find_package(Qt${QT_VERSION_MAJOR} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

if (NOT QT_PLUGINS_DIR)
    if (NOT USE_QT6)
        get_target_property(REAL_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE}
                            IMPORTED_LOCATION)
        execute_process(COMMAND "${REAL_QMAKE_EXECUTABLE}" -query QT_INSTALL_PLUGINS
                        OUTPUT_VARIABLE QT_PLUGINS_DIR
                        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        set(QT_PLUGINS_DIR ${QT6_INSTALL_PLUGINS})
    endif()
endif()

add_subdirectory(color-schemes)
add_subdirectory(src)
