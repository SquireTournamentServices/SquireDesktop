cmake_minimum_required(VERSION 3.9)

project(
  SquireDesktop
  VERSION 0.1
  LANGUAGES C CXX)

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
message(STATUS "Commit info: ${GIT_COMMIT_HASH}@${GIT_BRANCH}")

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

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions("-D__FILENAME__=(&__FILE__[SOURCE_PATH_SIZE])")

if(WIN32)
  add_definitions("-D__WINDOWS")
  set(CURL_STATIC_CRT ON)
  set(JANSSON_STATIC_CRT ON)

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
  add_definitions("-D__UNIX")
  if(APPLE)
    # macOS Due to the special bundle structure ignore the prefix eventually set
    # by the user.
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/release)
    add_definitions("-D__APPLE")
  else()
    add_definitions("-D__LINUX") # bsd isnt real
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
else()
  # Compiler settings compiler flags
  set(COMPILER_FLAGS "-Wall -Wextra -Wpedantic -gdwarf-4 -gsplit-dwarf")
  set(CMAKE_C_FLAGS_DEBUG "${COMPILER_FLAGS} -DDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
  set(CMAKE_C_FLAGS_RELEASE "${COMPILER_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
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

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)

# Squire Core (Rust)
# https://stackoverflow.com/questions/40743372/gcc-lto-appears-to-strip-debugging-symbols
option(ENABLE_LTO ON)
# option(ENABLE_LTO OFF)

# Installs Discord Game SDK binaries
set(DISCORD_GAME_SDK_PATH "${CMAKE_SOURCE_DIR}")
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set(CMAKE_DISCORD_GAME_SDK_LIB_PATH
      "${DISCORD_GAME_SDK_PATH}/lib/x86_64/discord_game_sdk.dll.lib")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set(CMAKE_DISCORD_GAME_SDK_LIB_PATH
      "${DISCORD_GAME_SDK_PATH}/lib/x86_64/discord_game_sdk.so")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Macos")
  set(CMAKE_DISCORD_GAME_SDK_LIB_PATH
      "${DISCORD_GAME_SDK_PATH}/lib/x86_64/discord_game_sdk.dylib")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set(CMAKE_DISCORD_GAME_SDK_LIB_PATH
      "${DISCORD_GAME_SDK_PATH}/lib/x86_64/discord_game_sdk.dylib")
endif()
install(FILES ${CMAKE_DISCORD_GAME_SDK_LIB_PATH}
        DESTINATION "${CMAKE_BINARY_DIR}/lib/")
message(STATUS "Discord game sdk: ${CMAKE_DISCORD_GAME_SDK_LIB_PATH}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
find_package(OpenSSL REQUIRED Crypto SSL)

# Setup deps
set(JSON_BuildTests
    OFF
    CACHE INTERNAL "")
set(CURL_STATICLIB ON)

include_directories(${CMAKE_BINARY_DIR}/squire_ffi ${JANSSON_INCLUDE_DIRS})
set(LIBS
    nlohmann_json::nlohmann_json
    mtg-search-engine
    jansson
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread) # backtrace)

if(WIN32)
  add_library(pthread STATIC IMPORTED)
  set_target_properties(
    pthread PROPERTIES IMPORTED_LOCATION
                       "${CMAKE_SOURCE_DIR}/lib/windows_compat/pthreadVC2.lib")
  include_directories(${CMAKE_SOURCE_DIR}/lib/windows_compat/)

  # These set statements are magical and from
  # https://github.com/google/re2/issues/436
  set(ABSL_PROPAGATE_CXX_STD ON)
  set(ABSL_ENABLE_INSTALL ON)
  add_subdirectory(./abseil-cpp)
else()
  set(LIBS ${LIBS} m)
endif()

# This feature is turned off at the moment add_subdirectory(libbacktrace)
add_subdirectory(nlohmann_json)
add_subdirectory(squire_ffi)
add_subdirectory(curl)
add_subdirectory(jansson)
add_subdirectory(src/mtg_search_engine)

# Non-ui files that are tested
set(TS_FILES SquireDesktop_en_GB.ts)
set(MAIN_FILES
    ./testing_h/logger.h
    ./testing_h/ansi_colour.h
    ./src/filerable_list.hpp
    ./src/utils.cpp
    ./src/utils.h
    ./src/coins.cpp
    ./src/coins.h
    ./src/config.cpp
    ./src/config.h
    ./src/search.h
    ./src/search.cpp
    ./src/timers.cpp
    ./src/timers.h)

set(FFI_FILES
    ./src/ffi_utils.cpp
    ./src/ffi_utils.h
    ./src/model/player.cpp
    ./src/model/player.h
    ./src/model/round.cpp
    ./src/model/round.h
    ./src/model/abstract_tournament.cpp
    ./src/model/abstract_tournament.h)

# Assets to data strings
file(COPY "assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
set(ASSETS_TO_STR_PY "${CMAKE_SOURCE_DIR}/assets_to_header.py")
set(ASSETS_FILE ${CMAKE_CURRENT_BINARY_DIR}/assets.h)
if(UNUX)
  add_custom_command(OUTPUT ${ASSETS_FILE} COMMAND python3 ${ASSETS_TO_STR_PY})
else()
  add_custom_command(OUTPUT ${ASSETS_FILE} COMMAND python ${ASSETS_TO_STR_PY})
endif()

set(PROJECT_SOURCES
    ${TS_FILES}
    ${MAIN_FILES}
    ${FFI_FILES}
    ${ASSETS_FILE}
    ./src/discord_game_sdk.h
    ./src/init.cpp
    ./src/init.h
    ./src/ui/mainwindow.cpp
    ./src/ui/mainwindow.h
    ./src/ui/mainwindow.ui
    ./src/ui/appdashboardtab.cpp
    ./src/ui/appdashboardtab.h
    ./src/ui/appdashboardtab.ui
    ./src/ui/abstracttabwidget.cpp
    ./src/ui/abstracttabwidget.h
    ./src/ui/tournamenttab.cpp
    ./src/ui/tournamenttab.h
    ./src/ui/tournamenttab.ui
    ./src/ui/tournament/addplayerdialogue.cpp
    ./src/ui/tournament/addplayerdialogue.h
    ./src/ui/tournament/addplayerdialogue.ui
    ./src/ui/tournament/tournamentchangesettingsdialogue.cpp
    ./src/ui/tournament/tournamentchangesettingsdialogue.h
    ./src/ui/tournament/tournamentchangesettingsdialogue.ui
    ./src/ui/tournament/tournamentunsavederrordialogue.cpp
    ./src/ui/tournament/tournamentunsavederrordialogue.h
    ./src/ui/tournament/tournamentunsavederrordialogue.ui
    ./src/ui/tournament/roundviewwidget.cpp
    ./src/ui/tournament/roundviewwidget.h
    ./src/ui/tournament/roundviewwidget.ui
    ./src/ui/tournament/roundresultwidget.cpp
    ./src/ui/tournament/roundresultwidget.h
    ./src/ui/tournament/roundresultwidget.ui
    ./src/ui/tournament/playerviewwidget.cpp
    ./src/ui/tournament/playerviewwidget.h
    ./src/ui/tournament/playerviewwidget.ui
    ./src/ui/tournament/standingsboardwidget.cpp
    ./src/ui/tournament/standingsboardwidget.h
    ./src/ui/tournament/standingsboardwidget.ui
    ./src/ui/tournament/playerlistwidget.h
    ./src/ui/tournament/playerlistwidget.cpp
    ./src/ui/tournament/playerlistwidget.ui
    ./src/ui/menubar/file/settingtab.cpp
    ./src/ui/menubar/file/settingtab.h
    ./src/ui/menubar/file/settingtab.ui
    ./src/ui/menubar/file/createtournamentdialogue.cpp
    ./src/ui/menubar/file/createtournamentdialogue.h
    ./src/ui/menubar/file/createtournamentdialogue.ui
    ./src/ui/menubar/file/random_names.cpp
    ./src/ui/menubar/file/random_names.h
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
    ./src/ui/widgets/labelimage.cpp
    ./src/ui/widgets/labelimage.h
    ./src/ui/widgets/recenttournamentwidget.cpp
    ./src/ui/widgets/recenttournamentwidget.h
    ./src/ui/widgets/recenttournamentwidget.ui
    ./src/ui/widgets/searchsorttablewidget.cpp
    ./src/ui/widgets/searchsorttablewidget.h
    ./src/ui/widgets/searchsorttablewidget.ui
    ./src/ui/widgets/tablemodel.hpp
    ./src/ui/abstractmodels/playermodel.cpp
    ./src/ui/abstractmodels/playermodel.h
    ./src/ui/abstractmodels/roundmodel.cpp
    ./src/ui/abstractmodels/roundmodel.h
    ./src/ui/abstractmodels/playerscoremodel.cpp
    ./src/ui/abstractmodels/playerscoremodel.h
    ./src/ui/abstractmodels/searchmodel.cpp
    ./src/ui/abstractmodels/searchmodel.h
    ./src/main.cpp)

set(TESTING_SOURCES
    ${MAIN_FILES}
    ./testing_h/testing.h
    ./tests/test_filter_list.cpp
    ./tests/test_filter_list.h
    ./tests/test_coins.cpp
    ./tests/test_coins.h
    ./tests/test_config.cpp
    ./tests/test_config.h
    ./tests/test_utils.cpp
    ./tests/test_utils.h
    ./tests/unit_tests.h
    ./tests/unit_tests.cpp
    ./tests/test_timers.cpp
    ./tests/test_timers.h
    ./tests/test_search.h
    ./tests/test_search.cpp)

set(FFI_TESTING_SOURCES
    ${MAIN_FILES}
    ${FFI_FILES}
    ./testing_h/testing.h
    ./tests_ffi/sq_link_test.h
    ./tests_ffi/sq_link_test.cpp
    ./tests_ffi/test_tournament_ffi.h
    ./tests_ffi/test_tournament_ffi.cpp
    ./tests_ffi/test_round_ffi.h
    ./tests_ffi/test_round_ffi.cpp
    ./tests_ffi/test_ffi.cpp
    ./tests_ffi/test_ffi.h)

# Make crash handler
if(UNIX)
  add_executable(
    sq_crash_handler
    ./crash_handler/crash_handler.c
    ./crash_handler/config_reader.c
    ./crash_handler/config_reader.h
    ./crash_handler/webhooks.c
    ./crash_handler/webhooks.h
    ./crash_handler/escape_seq.h
    ./crash_handler/escape_seq.c
    ./crash_handler/path_change.h
    ./crash_handler/path_change.c
    ./testing_h/ansi_colour.h
    ./testing_h/testing.h
    ./testing_h/logger.h)
  target_link_libraries(sq_crash_handler ${LIBS})
else()
  message(WARNING "The crash handler will not be built.")
endif()

# Build desktop app
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
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(SquireDesktop ${PROJECT_SOURCES})
  endif()

  qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(SquireDesktop PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                            squire_ffi ${LIBS})

# Link discord
add_library(discord STATIC IMPORTED)
set_target_properties(discord PROPERTIES IMPORTED_LOCATION
                                         ${CMAKE_DISCORD_GAME_SDK_LIB_PATH})
target_link_libraries(SquireDesktop PUBLIC discord)

set_target_properties(
  SquireDesktop
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER monarch.squire_desktop
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

add_custom_command(
  TARGET SquireDesktop
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${CMAKE_DISCORD_GAME_SDK_LIB_PATH}" $<TARGET_FILE_DIR:SquireDesktop>)

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(SquireDesktop)
endif()

# Make tests when needed
if(UNIX)
  include(CodeCoverage)

  # Setup coverage (using gcovr so needs gcc/g++) set(CMAKE_C_COMPILER
  # /usr/bin/gcc) set(CMAKE_CXX_COMPILER /usr/bin/g++)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -DTEST")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -DTEST")
  set(GCOVR_EXCLUDE "build/*" "tests/*" "tests_ffi/*" "src/main.cpp" "src/ui/*"
                    "coverage_main.cpp")

  append_coverage_compiler_flags()
  setup_target_for_coverage_gcovr_xml(
    NAME coverage EXECUTABLE SquireDesktopCoverage EXCLUDE ${GCOVR_EXCLUDE})

  # tests, set the flags manually
  set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_DEBUG})
  set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_DEBUG})

  add_executable(SquireDesktopTests ./tests/main.cpp ${TESTING_SOURCES})
  target_link_libraries(SquireDesktopTests PUBLIC ${LIBS})
  target_include_directories(SquireDesktopTests
                             PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

  add_executable(SquireDesktopFFITests ./tests_ffi/main.cpp
                                       ${FFI_TESTING_SOURCES})
  target_link_libraries(
    SquireDesktopFFITests
    PUBLIC ${LIBS}
    PRIVATE squire_ffi Qt${QT_VERSION_MAJOR}::Widgets)

  add_executable(SquireDesktopCoverage
                 ./coverage_main.cpp ${FFI_TESTING_SOURCES} ${TESTING_SOURCES})
  target_link_libraries(
    SquireDesktopCoverage
    PUBLIC ${LIBS}
    PRIVATE squire_ffi Qt${QT_VERSION_MAJOR}::Widgets)

  file(COPY config_test_tournament.tourn DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY mem_tests.sh DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY mem_tests.py DESTINATION ${CMAKE_BINARY_DIR})

  enable_testing()
  add_test(NAME main-tests COMMAND SquireDesktopTests)
  add_test(NAME memory-tests COMMAND bash mem_tests.sh)
  add_test(NAME ffi-tests COMMAND SquireDesktopFFITests)
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

# Package with cpack, this is the same code as Cockatrice
include(CPack)
