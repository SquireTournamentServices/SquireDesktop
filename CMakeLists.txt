cmake_minimum_required(VERSION 3.9)

project(
  SquireDesktop
  VERSION 0.1
  LANGUAGES CXX)

set(REPO_URL "https://github.com/MonarchDevelopment/SquireDesktop")

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
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Version info
add_definitions("-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")
add_definitions("-DGIT_BRANCH=\"${GIT_BRANCH}\"")
add_definitions(
  "-DVERSION=\"${CMAKE_PROJECT_VERSION} (${GIT_COMMIT_HASH}@${GIT_BRANCH})\"")
add_definitions("-DOS=\"${CMAKE_SYSTEM_NAME}\"")
add_definitions("-DREPO_URL=\"${REPO_URL}\"")
add_definitions("-DBUFFER_LENGTH=4096")
add_definitions("-DPROJECT_VERSION=\"${CMAKE_PROJECT_VERSION}\"")
add_definitions("-DPROJECT_NAME=\"${CMAKE_PROJECT_NAME}\"")

# __FILENAME__
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions("-D__FILENAME__=(__FILE__ + SOURCE_PATH_SIZE)")

if(WIN32)
  message(STATUS "Not using backtrace catcher on windoze")
else()
  add_definitions("-DUSE_BACKTRACE")
endif()

if(WIN32)
  # Use vcpkg toolchain on Windows
  set(CMAKE_TOOLCHAIN_FILE
      ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
      CACHE STRING "Vcpkg toolchain file")
  # Qt path set by user or env var
  if(QTDIR
     OR DEFINED ENV{QTDIR}
     OR DEFINED ENV{QTDIR32}
     OR DEFINED ENV{QTDIR64})

  else()
    set(QTDIR
        ""
        CACHE PATH "Path to Qt (e.g. C:/Qt/5.7/msvc2015_64)")
    message(
      WARNING
        "QTDIR variable is missing.  Please set this variable to specify path to Qt (e.g. C:/Qt/5.7/msvc2015_64)"
    )
  endif()
endif()

# Define a proper install path
if(UNIX)
  if(APPLE)
    # macOS Due to the special bundle structure ignore the prefix eventually set
    # by the user.
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/release)

    # Force ccache usage if available
    get_property(RULE_LAUNCH_COMPILE GLOBAL PROPERTY RULE_LAUNCH_COMPILE)
    if(RULE_LAUNCH_COMPILE)
      message(STATUS "Force enabling CCache usage under macOS")
      # Set up wrapper scripts
      configure_file("${COCKATRICE_CMAKE_PATH}/launch-c.in" launch-c)
      configure_file("${COCKATRICE_CMAKE_PATH}/launch-cxx.in" launch-cxx)
      execute_process(COMMAND chmod a+rx "${CMAKE_BINARY_DIR}/launch-c"
                              "${CMAKE_BINARY_DIR}/launch-cxx")

      # Set Xcode project attributes to route compilation through our scripts
      set(CMAKE_XCODE_ATTRIBUTE_CC "${CMAKE_BINARY_DIR}/launch-c")
      set(CMAKE_XCODE_ATTRIBUTE_CXX "${CMAKE_BINARY_DIR}/launch-cxx")
      set(CMAKE_XCODE_ATTRIBUTE_LD "${CMAKE_BINARY_DIR}/launch-c")
      set(CMAKE_XCODE_ATTRIBUTE_LDPLUSPLUS "${CMAKE_BINARY_DIR}/launch-cxx")
    endif()
  else()
    # Linux / BSD
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
      # fix package build
      if(PREFIX)
        set(CMAKE_INSTALL_PREFIX ${PREFIX})
      else()
        set(CMAKE_INSTALL_PREFIX /usr/local)
      endif()
    endif()
  endif()
elseif(WIN32)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/rundir/${CMAKE_BUILD_TYPE})
endif()

# Define proper compilation flags
if(MSVC)
  # Visual Studio: Maximum optimization, disable warning C4251, establish C++17
  # compatibility
  set(CMAKE_CXX_FLAGS_RELEASE
      "/Ox /MD /wd4251 /Zc:__cplusplus /std:c++17 /permissive-")
  # Generate complete debugging information set(CMAKE_CXX_FLAGS_DEBUG "/Zi")
elseif(CMAKE_COMPILER_IS_GNUCXX)
  # linux/gcc, bsd/gcc, windows/mingw
  include(CheckCXXCompilerFlag)

  set(CMAKE_CXX_FLAGS_RELEASE "-s -O2")
  if(WARNING_AS_ERROR)
    set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O0 -Wall -Wextra -Werror")
  else()
    set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O0 -Wall -Wextra")
  endif()

  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")
  endif()

  set(ADDITIONAL_DEBUG_FLAGS
      -Wcast-align
      -Wmissing-declarations
      -Wno-long-long
      -Wno-error=extra
      -Wno-error=delete-non-virtual-dtor
      -Wno-error=sign-compare
      -Wno-error=missing-declarations)

  foreach(FLAG ${ADDITIONAL_DEBUG_FLAGS})
    check_cxx_compiler_flag("${FLAG}" CXX_HAS_WARNING_${FLAG})
    if(CXX_HAS_WARNING_${FLAG})
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${FLAG}")
    endif()
  endforeach()
else()
  # other: osx/llvm, bsd/llvm
  set(CMAKE_CXX_FLAGS_RELEASE "-O2")
  if(WARNING_AS_ERROR)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Werror")
  else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
  endif()
endif()

if(DEFINED QTDIR${_lib_suffix})
  list(APPEND CMAKE_PREFIX_PATH "${QTDIR${_lib_suffix}}")
elseif(DEFINED QTDIR)
  list(APPEND CMAKE_PREFIX_PATH "${QTDIR}")
elseif(DEFINED ENV{QTDIR${_lib_suffix}})
  list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR${_lib_suffix}}")
elseif(DEFINED ENV{QTDIR})
  list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()

# Compiler settings compiler flags
set(COMPILER_FLAGS "-Wall -Wextra -Wpedantic -std=gnu++98")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 ${COMPILER_FLAGS} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_C_FLAGS_RELEASE "-O2 ${COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES SquireDesktop_en_GB.ts)

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
    ./src/io_utils.h)

set(PROJECT_SOURCES
    ${TS_FILES}
    ${MAIN_FILES}
    ./src/ui/mainwindow.cpp
    ./src/ui/mainwindow.h
    ./src/ui/mainwindow.ui
    ./src/ui/appdashboardtab.cpp
    ./src/ui/appdashboardtab.h
    ./src/ui/appdashboardtab.ui
    ./src/ui/recenttournamentwidget.cpp
    ./src/ui/recenttournamentwidget.h
    ./src/ui/recenttournamentwidget.ui
    ./src/ui/menubar/file/settingtab.cpp
    ./src/ui/menubar/file/settingtab.h
    ./src/ui/menubar/file/settingtab.ui
    ./src/ui/menubar/rng/coinsflipdialogue.cpp
    ./src/ui/menubar/rng/coinsflipdialogue.h
    ./src/ui/menubar/rng/coinsflipdialogue.ui
    ./src/ui/menubar/rng/dicerolldialogue.cpp
    ./src/ui/menubar/rng/dicerolldialogue.h
    ./src/ui/menubar/rng/dicerolldialogue.ui
    ./src/ui/menubar/rng/dicerollresultwidget.cpp
    ./src/ui/menubar/rng/dicerollresultwidget.h
    ./src/ui/menubar/rng/dicerollresultwidget.ui
    ./src/ui/menubar/rng/dicerollresultdialogue.cpp
    ./src/ui/menubar/rng/dicerollresultdialogue.h
    ./src/ui/menubar/rng/dicerollresultdialogue.ui
    ./src/main.cpp)

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
    ./tests/sq_link_test.h
    ./tests/sq_link_test.cpp)

set(LIBS "m" # Maths
)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)

# Assets to data strings
file(COPY "assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
set(ASSETS_TO_STR_PY "${CMAKE_SOURCE_DIR}/assets_to_header.py")
add_custom_target(generate_assets ${ASSETS_TO_STR_PY})

# Squire Core (Rust)
option(ENABLE_LTO ON)
if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

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

add_subdirectory(squire_core)

include_directories(${CMAKE_BINARY_DIR}/squire_core)
include_directories(include)

if(CMAKE_BUILD_TYPE STREQUAL "TEST")
  include(CodeCoverage)

  # Setup coverage (using gcovr so needs gcc/g++) set(CMAKE_C_COMPILER
  # /usr/bin/gcc) set(CMAKE_CXX_COMPILER /usr/bin/g++)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -DTEST")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -DTEST")

  append_coverage_compiler_flags()
  setup_target_for_coverage_gcovr_xml(
    NAME
    coverage
    EXECUTABLE
    SquireDesktopTests
    EXCLUDE
    "build/*"
    "tests/*"
    "src/main.cpp"
    "src/ui/*")
endif()

# Setup deps
set(JSON_BuildTests
    OFF
    CACHE INTERNAL "")
add_subdirectory(nlohmann_json)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(SquireDesktop MANUAL_FINALIZATION ${PROJECT_SOURCES})
  # Define target properties for Android with Qt 6 as: set_property(TARGET
  # SquireDesktop APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
  # ${CMAKE_CURRENT_SOURCE_DIR}/android) For more information, see
  # https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

  qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
  if(ANDROID)
    add_library(SquireDesktop SHARED ${PROJECT_SOURCES})
    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(SquireDesktop ${PROJECT_SOURCES})
  endif()

  qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(
  SquireDesktop PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                        nlohmann_json::nlohmann_json squire_core)
set_target_properties(
  SquireDesktop
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER monarch.squire_desktop
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)
add_dependencies(SquireDesktop generate_assets)

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(SquireDesktop)
endif()

# Make tests when needed
if(CMAKE_BUILD_TYPE STREQUAL "TEST")
  # tests, set the flags manually
  set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_DEBUG})
  set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_DEBUG})

  add_executable(SquireDesktopTests ${TESTING_SOURCES})
  target_link_libraries(
    SquireDesktopTests
    PUBLIC ${LIBS}
    PRIVATE nlohmann_json::nlohmann_json squire_core)
  target_include_directories(SquireDesktopTests
                             PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

  file(COPY mem_tests.sh DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY mem_tests.py DESTINATION ${CMAKE_BINARY_DIR})

  enable_testing()
  add_test(NAME tests COMMAND SquireDesktopTests)
  add_test(NAME memory-tests COMMAND bash mem_tests.sh)
  message(STATUS "TEST build setup")
endif()

# Package builder
set(CPACK_PACKAGE_CONTACT "Danny Piper <djpiper28@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Monarch Development")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_VERSION_FILENAME}")

if(UNIX)
  if(APPLE)
    set(CPACK_GENERATOR DragNDrop ${CPACK_GENERATOR})
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_FORMAT "UDBZ")
    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
    set(CPACK_SYSTEM_NAME "OSX")
    set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.png"
    )# does it need to be icns?
  else()
    # linux
    if(CPACK_GENERATOR STREQUAL "RPM")
      set(CPACK_RPM_PACKAGE_LICENSE "AGPLv3")
      set(CPACK_RPM_MAIN_COMPONENT "squire_desktop")
      if(Qt6_FOUND)
        set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qttools, qt6-qtmultimedia")
      elseif(Qt5_FOUND)
        set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qttools, qt5-qtmultimedia")
      endif()
      set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
      set(CPACK_RPM_PACKAGE_URL REPO_URL)
      # stop directories from making package conflicts
      set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
          /usr/share/applications
          /usr/share/icons
          /usr/share/icons/hicolor
          /usr/share/icons/hicolor/48x48
          /usr/share/icons/hicolor/48x48/apps
          /usr/share/icons/hicolor/scalable
          /usr/share/icons/hicolor/scalable/apps)
    else()
      set(CPACK_GENERATOR DEB)
      set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
      set(CPACK_DEBIAN_PACKAGE_SECTION "games")
      set(CPACK_DEBIAN_PACKAGE_HOMEPAGE REPO_URL)
      if(Qt6_FOUND)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6multimedia6, qt6-qpa-plugins")
      elseif(Qt5_FOUND)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5multimedia5-plugins")
      endif()
    endif()
  endif()
elseif(WIN32)
  set(CPACK_GENERATOR NSIS ${CPACK_GENERATOR})
  if("${CMAKE_GENERATOR_PLATFORM}" MATCHES "(x64)")
    set(TRICE_IS_64_BIT 1)
  else()
    set(TRICE_IS_64_BIT 0)
  endif()

  # include vcredist into the package; NSIS will take care of running it
  if(VCREDISTRUNTIME_FOUND)
    install(FILES "${VCREDISTRUNTIME_FILE}" DESTINATION ./)
  endif()
endif()

include(CPack)
