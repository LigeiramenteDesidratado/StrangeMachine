cmake_minimum_required(VERSION 3.5)

project(StrangeMachineMath LANGUAGES C)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "${PROJECT_NAME}:")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}" )
message(STATUS "C flags, Debug configuration: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "C flags, Release configuration: ${CMAKE_C_FLAGS_RELEASE}")

list(APPEND flags "-fPIC" "-Wall")
if(NOT WIN32)
  list(APPEND flags "-Wextra" "-Wpedantic")
endif()
message(STATUS "C flags: ${CMAKE_C_FLAGS}")


add_library(${PROJECT_NAME} STATIC 
  mat4.c
  quat.c
  scalar.c
  transform.c
  vec3.c
)

target_link_libraries(${PROJECT_NAME} PUBLIC m)
