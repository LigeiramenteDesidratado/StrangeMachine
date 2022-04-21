cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_COMPILER "/usr/bin/clang")

project(StrangeMachine LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(BUILD_EXAMPLES "build examples (default on)" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Release flags: ${CMAKE_C_FLAGS_RELEASE}")

set(SM_FLAGS_DEBUG_FLAGS "-g3 -fPIC -Wall -flto -pipe -fvisibility=hidden -fcf-protection=full -Werror=format-security  -Werror=implicit-function-declaration -Wbad-function-cast")
set(SM_FLAGS_RELEASE_FLAGS "-O3 -DNDEBUG -fPIC -Wall")

if(NOT WIN32)
  list(APPEND SM_FLAGS_DEBUG_FLAGS "-Wextra -Wpedantic")
  list(APPEND SM_FLAGS_RELEASE_FLAGS "-Wextra -Wpedantic")
endif()

list(JOIN SM_FLAGS_DEBUG_FLAGS " " SM_FLAGS_DEBUG_FLAGS)
list(JOIN SM_FLAGS_RELEASE_FLAGS " " SM_FLAGS_RELEASE_FLAGS)

SET(CMAKE_C_FLAGS_DEBUG " ${SM_FLAGS_DEBUG_FLAGS}")
SET(CMAKE_C_FLAGS_RELEASE " ${SM_FLAGS_RELEASE_FLAGS}")


add_subdirectory(StrangeMachine)
if (BUILD_EXAMPLES)
  #  add_subdirectory(examples/terror-em-sl)
  # add_subdirectory(examples/lab)
  add_subdirectory(TestSites/debug)
  add_subdirectory(TestSites/entity)
endif()
