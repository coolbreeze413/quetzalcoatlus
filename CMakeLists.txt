
cmake_minimum_required(VERSION 3.14)

message("\n\n")


if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX  ${CMAKE_CURRENT_SOURCE_DIR}/install)
endif(NOT DEFINED CMAKE_INSTALL_PREFIX)

message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")


project(quetzalcoatlus LANGUAGES CXX)


# getting CMake to find the correct Qt packages...
# recommend user to install new Qt version using installer to "/home/${USER}/qt"
# pass in the CMAKE_PREFIX_PATH to be used by us from outside, i.e. the Makefile wrapper.
# the following sections are only here for additional information.

# rabbit hole 0: this works, reports correct Qt_VERSION from find_packages
#                *and* includes correct headers, and links correct libraries
# use this line and *modify* it for the system *only* if we are using non-distro-provided Qt!
# *note* that, for non-distro-provided Qt, running application will need the LD_LIBRARY_PATH set as well:
# example: LD_LIBRARY_PATH=/home/${USER}/qt/5.15.2/gcc_64/lib:$LD_LIBRARY_PATH ./install/bin/squintymongrel
#
# this path should be the 'root' path for the correct platform, containing the bin/ and lib/ directories
# list(PREPEND CMAKE_PREFIX_PATH "/home/$ENV{USER}/qt/5.15.2/gcc_64")
# list(PREPEND CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/qt5")

# rabbit hole 1: does not work
# even with Qt_DIR or Qt5_DIR or other variations
# set(Qt_DIR "/home/$ENV{USER}/qt/5.15.2/gcc_64/lib/cmake/Qt5" CACHE PATH "Initial cache" FORCE)
# set(Qt_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt5")

# rabbit hole 2: does not work
# the following works i.e. includes and link libraries are correct, but strangely 
#     does *not* affect the *version* reported by find_package!
# set(Qt5_ROOT "/home/$ENV{USER}/qt/5.15.2/gcc_64" CACHE PATH "Initial cache" FORCE)
# so we also set the version here.
# set(Qt_VERSION 5 15 2)


# note that find_package() populates the package_VERSION variable, as well as packagecomponent_VERSION variable.
# this can be used for further checks as below.
find_package(Qt NAMES Qt5 Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${Qt_VERSION_MAJOR} COMPONENTS Core Gui Widgets WebEngineWidgets WebChannel REQUIRED)
# package_VERSION
message("Qt Version: " ${Qt_VERSION})
# message("Qt${Qt_VERSION_MAJOR} Version: " ${Qt${Qt_VERSION_MAJOR}_VERSION})
# packagecomponent_VERSION
# message("Qt${Qt_VERSION_MAJOR}Core Version: " ${Qt${Qt_VERSION_MAJOR}Core_VERSION})
# message("Qt${Qt_VERSION_MAJOR}Widgets Version: " ${Qt${Qt_VERSION_MAJOR}Widgets_VERSION})


# https://itecnote.com/tecnote/qt-test-for-supported-qt-version-with-cmake/
# https://cmake.org/cmake/help/latest/variable/CMAKE_VERSION.html
if (Qt_FOUND)
    if (Qt_VERSION VERSION_GREATER 5.12.8)
        # for Qt 5.15.2: https://bugreports.qt.io/browse/QTBUG-80964
        if (UNIX)
            message("\n  Qt_VERSION > 5.12.8: " ${Qt_VERSION} ", enabling gold linker!")
            set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=gold")
        endif()
    endif()
endif()


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
    Qt${Qt_VERSION_MAJOR}::Core
    Qt${Qt_VERSION_MAJOR}::Gui
    Qt${Qt_VERSION_MAJOR}::Widgets
)

# compile time definitions for target:

# pass in the date and time
target_compile_definitions(quetzalcoatlus PUBLIC BUILD_DATE=${BUILD_DATE})
target_compile_definitions(quetzalcoatlus PUBLIC BUILD_TIME=${BUILD_TIME})

# pass in the version
target_compile_definitions(quetzalcoatlus PUBLIC BUILD_VERSION=${BUILD_VERSION})

# pass in the git details
target_compile_definitions(quetzalcoatlus PUBLIC BUILD_GIT_REPO_URL=${BUILD_GIT_REPO_URL})
target_compile_definitions(quetzalcoatlus PUBLIC BUILD_GIT_HASH=${BUILD_GIT_HASH})

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

install(
    DIRECTORY
        testfiles
    DESTINATION
        share/quetzalcoatlus/
)

message("\n\n")
