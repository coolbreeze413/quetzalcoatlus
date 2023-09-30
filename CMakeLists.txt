
cmake_minimum_required(VERSION 3.14)


if(NOT DEFINED QUETZALCOATLUS_VERSION)
    set(QUETZALCOATLUS_VERSION "v0.0.0")
endif()

if(NOT DEFINED QUETZALCOATLUS_GIT_HASH)
    set(QUETZALCOATLUS_GIT_HASH "zyxwvutsrqponmlkjihgfedcba")
endif()

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX  ${CMAKE_CURRENT_SOURCE_DIR}/install)
endif(NOT DEFINED CMAKE_INSTALL_PREFIX)

message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message("QUETZALCOATLUS_VERSION: ${QUETZALCOATLUS_VERSION}")
message("QUETZALCOATLUS_GIT_HASH: ${QUETZALCOATLUS_GIT_HASH}")


project(quetzalcoatlus LANGUAGES CXX)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
# logging level for debug
set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")
# silent installation
set(CMAKE_INSTALL_MESSAGE NEVER)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS
#     "${CMAKE_CXX_FLAGS} ${CUSTOM_CXX_WARNING_FLAGS}")


set(SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/window.cpp
    ${PROJECT_SOURCE_DIR}/src/window.h
)

set(QRC_FILES
    ${PROJECT_SOURCE_DIR}/quetzalcoatlus.qrc
)

# define the 'executable' target
add_executable(quetzalcoatlus
    ${SOURCE_FILES}
    ${QRC_FILES}
)

# can set a custom output_name for the binary file produced
set_target_properties(quetzalcoatlus PROPERTIES OUTPUT_NAME quetzalcoatlus)

# include dirs for target
target_include_directories(quetzalcoatlus
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src)

# libs for target
target_link_libraries(quetzalcoatlus PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

# compile time definitions for target
# to test how it would work when compiled on a platform with no tray icon support
#target_compile_definitions(quetzalcoatlus PUBLIC QT_NO_SYSTEMTRAYICON=1)

# test passing in configuration define via CMake
#target_compile_definitions(quetzalcoatlus PUBLIC QUETZALCOATLUS_USE_SPLASH_SCREEN=0)

# use QGridLayout to layout stuff or not
target_compile_definitions(quetzalcoatlus PUBLIC QUETZALCOATLUS_USE_QGRIDLAYOUT=0)


# debug:
# message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
# message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
# message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)


install(TARGETS quetzalcoatlus
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(
    DIRECTORY
        testfiles
    DESTINATION
        share/quetzalcoatlus/
)

install(
    FILES
        resources/quetzalcoatlus.desktop
    DESTINATION
        share/applications
    PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
)

install(
    FILES
        resources/images/quetzalcoatlus_flying_outline_256x256.png
    DESTINATION
        share/icons/hicolor/256x256/apps
    RENAME
        quetzalcoatlus.png
    PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
)
