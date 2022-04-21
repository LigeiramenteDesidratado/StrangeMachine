cmake_minimum_required(VERSION 3.5)

project(DEBUG LANGUAGES C)

message(STATUS "${PROJECT_NAME}:")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}" )
message(STATUS "Debug cfg: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "Release cfg: ${CMAKE_C_FLAGS_RELEASE}")


add_executable(${PROJECT_NAME} debug.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/StrangeMachine/)

target_link_libraries(${PROJECT_NAME} PUBLIC StrangeMachineEngine)
