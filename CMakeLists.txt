cmake_minimum_required(VERSION 3.9)

project(SquireDesktop VERSION 0.1 LANGUAGES CXX)

set(REPO_URL "https://github.com/MonarchDevelopment/SquireDesktop")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compiler flags
set(COMPILER_FLAGS "-Wall -Wextra -Wpedantic -std=gnu++98")
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

# Non-ui files that are tested
set(MAIN_FILES
    ./testing_h/logger.cpp
    ./testing_h/logger.h
    ./testing_h/ansi_colour.h
    ./src/coins.cpp
    ./src/coins.h
    ./src/config.cpp
    ./src/config.h
    ./src/io_utils.cpp
    ./src/io_utils.h
)

set(PROJECT_SOURCES
    ${TS_FILES}
    ${MAIN_FILES}
    ./src/ui/mainwindow.cpp
    ./src/ui/mainwindow.h
    ./src/ui/mainwindow.ui
    ./src/main.cpp
)

set(TESTING_SOURCES
    ${MAIN_FILES}
    ./testing_h/testing.cpp
    ./testing_h/testing.h
    ./tests/main.cpp
    ./tests/test_coins.cpp
    ./tests/test_coins.h
    ./tests/test_config.cpp
    ./tests/test_config.h
    ./tests/test_io_utils.cpp
    ./tests/test_io_utils.h
)

set(LIBS
  "m" # Maths
)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)

# Squire Core (Rust)
option(ENABLE_LTO ON)
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif ()

option(ENABLE_LTO "Enable cross language linking time optimization" ON)
if(ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT supported OUTPUT error)
  if(supported)
      message(STATUS "IPO / LTO enabled")
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
      add_link_options(-fuse-ld=lld)
  else()
      message(STATUS "IPO / LTO not supported: <${error}>")
  endif()
endif()

include_directories(${CMAKE_BINARY_DIR}/squire_core)
include_directories(include)

add_subdirectory(squire_core)

if (CMAKE_BUILD_TYPE STREQUAL "TEST")
    include(CodeCoverage)

    # Setup coverage (using gcovr so needs gcc/g++)
                #set(CMAKE_C_COMPILER /usr/bin/gcc)
                #set(CMAKE_CXX_COMPILER /usr/bin/g++)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -DTEST")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -DTEST")

    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_xml(NAME coverage
      EXECUTABLE SquireDesktopTests
        EXCLUDE "build/*" "tests/*" "src/main.cpp" "src/ui/*")
endif()

# Setup deps
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(nlohmann_json)

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

target_link_libraries(SquireDesktop PRIVATE Qt${QT_VERSION_MAJOR}::Widgets nlohmann_json::nlohmann_json squire_core)

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

# Make tests when needed
if (CMAKE_BUILD_TYPE STREQUAL "TEST")
    # tests, set the flags manually
    set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_DEBUG})
    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_DEBUG})

    add_executable(SquireDesktopTests ${TESTING_SOURCES})
    target_link_libraries(SquireDesktopTests PUBLIC ${LIBS} PRIVATE nlohmann_json::nlohmann_json)
    target_include_directories(SquireDesktopTests PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

    file(COPY mem_tests.sh DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY mem_tests.py DESTINATION ${CMAKE_BINARY_DIR})

    enable_testing ()
    add_test(NAME tests COMMAND SquireDesktopTests)
    add_test(NAME memory-tests COMMAND bash mem_tests.sh)
    message(STATUS "TEST build setup")
endif()

