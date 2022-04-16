cmake_minimum_required(VERSION 3.5)

project(Entity LANGUAGES C)

message(STATUS "C flags: ${CMAKE_C_FLAGS}")

add_executable(${PROJECT_NAME} main.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/engine/)

target_link_libraries(${PROJECT_NAME} PUBLIC StrangeMachineEngine dl)
