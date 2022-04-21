cmake_minimum_required(VERSION 3.5)

project(TerrorEmSL LANGUAGES C)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})


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

add_executable(${PROJECT_NAME} main.c
  center.c
  player.c
  scene.c
  stage.c
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/StrangeMachine/)
include_directories(${CMAKE_SOURCE_DIR}/StrangeMachine/include/)
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} PUBLIC StrangeMachineEngine dl)
