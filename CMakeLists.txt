cmake_minimum_required(VERSION 3.5)

project(StrangeMachine LANGUAGES C)

option(BUILD_EXAMPLES "build examples (default on)" ON)

add_subdirectory(engine)
if (BUILD_EXAMPLES)
  add_subdirectory(examples/terror-em-sl)
  # Temporary
  file(COPY ps1.png  goth.png  goth.glb  plane.obj  plane.mtl DESTINATION .)
endif()
