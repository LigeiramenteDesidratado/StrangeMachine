cmake_minimum_required(VERSION 3.5)

project(world3D LANGUAGES C)

message(STATUS "C flags: ${CMAKE_C_FLAGS}")

add_executable(${PROJECT_NAME} main.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/StrangeMachine/)

target_link_libraries(${PROJECT_NAME} PUBLIC StrangeMachineEngine dl)
