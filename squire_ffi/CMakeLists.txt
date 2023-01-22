cmake_minimum_required(VERSION 3.9)

project(
  SquireFFFI
  VERSION 0.1
  LANGUAGES CXX)

set(SQ_REPO_URL "https://github.com/MonarchDevelopment/SquireFFI")

message(STATUS "SquireFFI information:")
message(STATUS "Building ${CMAKE_PROJECT_NAME}")
message(STATUS "SQ Git repo: ${SQ_REPO_URL}")
message(STATUS "SQ Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "SQ Version: ${CMAKE_PROJECT_VERSION}")
message(STATUS "SQ Build OS: ${CMAKE_SYSTEM_NAME}")

# Copy into your CMakeFile after importing this sub directory Get the branch
# name
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE SQ_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE SQ_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "SQ Commit info: ${SQ_GIT_COMMIT_HASH}@${SQ_GIT_BRANCH}")

# Version info
add_definitions("-DSQ_GIT_COMMIT_HASH=\"${SQ_GIT_COMMIT_HASH}\"")
add_definitions("-DSQ_GIT_BRANCH=\"${SQ_GIT_BRANCH}\"")
add_definitions(
  "-DSQ_VERSION=\"${CMAKE_PROJECT_VERSION} (${SQ_GIT_COMMIT_HASH}@${SQ_GIT_BRANCH})\""
)

find_package(OpenSSL)

if(WIN32)
  set(EXT "lib")
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Macos")
  set(EXT "dylib")
else()
  set(EXT "a")
endif()

set(RUST_PART_CXX_FILE "${CMAKE_CURRENT_BINARY_DIR}/squire_ffi.${EXT}")
set(RUST_PART_H_FILE "${CMAKE_CURRENT_BINARY_DIR}/squire_ffi.h")
message(
  STATUS
    "Building rust with options: ${RUST_FLAGS}, linking ${RUST_PART_CXX_FILE}")
if(WIN32)
  add_custom_command(
    OUTPUT ${RUST_PART_H_FILE} ${RUST_PART_CXX_FILE}
    COMMAND ${CMAKE_COMMAND} -E env RUSTFLAGS="${RUST_FLAGS}" "ffi_build.bat"
    COMMAND
      ${CMAKE_COMMAND} -E copy
      "${CMAKE_CURRENT_SOURCE_DIR}/target/release/squire_ffi.${EXT}"
      "${RUST_PART_CXX_FILE}"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/squire_ffi.h"
            "${RUST_PART_H_FILE}"
    COMMAND ${CMAKE_COMMAND} -E copy "${RUST_PART_CXX_FILE}"
            "${CMAKE_BINARY_DIR}/squire_ffi.${EXT}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
else()
  add_custom_command(
    OUTPUT ${RUST_PART_H_FILE} ${RUST_PART_CXX_FILE}
    COMMAND ${CMAKE_COMMAND} -E env RUSTFLAGS="${RUST_FLAGS}" bash
            "ffi_build.sh"
    COMMAND
      ${CMAKE_COMMAND} -E copy
      "${CMAKE_CURRENT_SOURCE_DIR}/target/release/libsquire_ffi.${EXT}"
      "${RUST_PART_CXX_FILE}"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/squire_ffi.h"
            "${RUST_PART_H_FILE}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
add_custom_target(RUST_PART_H DEPENDS ${RUST_PART_H_FILE})

set(LIBS)
if(UNIX)
  set(LIBS "dl" "pthread")
elseif(WIN32)
  add_link_options(/SUBSYSTEM:WINDOWS /DYNAMICBASE Version.lib Comctl32.lib
                   /MACHINE:X64)

  set(LIBS "wsock32" "ws2_32" "bcrypt")
endif()

add_library(squire_ffi INTERFACE)
set_target_properties(squire_ffi PROPERTIES IMPORTED_LOCATION
                                             ${RUST_PART_CXX_FILE})
target_link_libraries(squire_ffi INTERFACE ${RUST_PART_CXX_FILE} ${LIBS}
                                            OpenSSL::SSL OpenSSL::Crypto)
add_dependencies(squire_ffi RUST_PART_H)

add_test(
  NAME squire_ffi_test
  COMMAND cargo test
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "SquireFFI is being used")