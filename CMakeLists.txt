cmake_minimum_required(VERSION 3.5)

project(SquireDesktop VERSION 0.1 LANGUAGES CXX)

set(REPO_URL "https://github.com/MonarchDevelopment/SquireDesktop")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compiler flags
set(COMPILER_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 ${COMPILER_FLAGS} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_C_FLAGS_RELEASE "-O2 ${COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})

message(STATUS "Building ${CMAKE_PROJECT_NAME}")
message(STATUS "Git repo: ${REPO_URL}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Version: ${CMAKE_PROJECT_VERSION}")
message(STATUS "Build OS: ${CMAKE_SYSTEM_NAME}")

# Get the branch name
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Version info
add_definitions("-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")
add_definitions("-DGIT_BRANCH=\"${GIT_BRANCH}\"")
add_definitions("-DVERSION=\"${CMAKE_PROJECT_VERSION} (${GIT_COMMIT_HASH}@${GIT_BRANCH})\"")
add_definitions("-DOS=\"${CMAKE_SYSTEM_NAME}\"")
add_definitions("-DREPO_URL=\"${REPO_URL}\"")
add_definitions("-DBUFFER_LENGTH=4096")

# __FILENAME__
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions("-D__FILENAME__=(__FILE__ + SOURCE_PATH_SIZE)")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES
    SquireDesktop_en_GB.ts
)

set(MAIN_FILES
    ./mainwindow.cpp
    ./mainwindow.h
    ./mainwindow.ui
    ./testing_h/logger.cpp
    ./testing_h/logger.h
)

set(PROJECT_SOURCES
    ${TS_FILES}
    ${MAIN_FILES}
    ./main.cpp
)

set(TESTING_SOURCES
    ${MAIN_FILES}
    ./testing_h/ansi_colour.h
    ./testing_h/testing.cpp
    ./testing_h/testing.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SquireDesktop
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SquireDesktop APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(SquireDesktop SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SquireDesktop
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(SquireDesktop PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(SquireDesktop PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SquireDesktop)
endif()
