cmake_minimum_required(VERSION 3.5)

project(StrangeMachine LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(BUILD_EXAMPLES "build examples (default on)" ON)

add_subdirectory(engine)
if (BUILD_EXAMPLES)
  #  add_subdirectory(examples/terror-em-sl)
  add_subdirectory(examples/lab)
  add_subdirectory(examples/debug)
endif()
