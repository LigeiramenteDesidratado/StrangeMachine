cmake_minimum_required(VERSION 3.21.4)

project(StrangeMachineEngine LANGUAGES C)

find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "${PROJECT_NAME}:")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}" )
message(STATUS "C flags, Debug configuration: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "C flags, Release configuration: ${CMAKE_C_FLAGS_RELEASE}")

add_library(${PROJECT_NAME} STATIC 
  glad.c
  smAttribute.c
  smCamera.c
  smClip.c
  smController.c
  smCrossFadeTarget.c
  smDebug.c
  smGLTFLoader.c
  smIndexBuffer.c
  smInput.c
  smMesh.c
  smModel.c
  smOBJLoader.c
  smPhysics.c
  smPose.c
  smRearrangeBones.c
  smShader.c
  smShaderProgram.c
  smShapes.c
  smSkeleton.c
  smSkinnedMesh.c
  smSkinnedModel.c
  smText.c
  smTexture.c
  smTrack.c
  smTransformTrack.c
  smUniform.c
  smLoader.c
)

target_compile_options(${PROJECT_NAME} PRIVATE -fPIC -Wall)
if(NOT WIN32)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wextra -Wpedantic)
endif()


add_subdirectory(math)
target_include_directories(${PROJECT_NAME} PUBLIC "include" ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC StrangeMachineMath SDL2 m)
# Copy content to build folder:
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  # Copy shader folder
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/glsl ${CMAKE_CURRENT_BINARY_DIR}/glsl
  # Copy font atlas
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/smFonts.png ${CMAKE_CURRENT_BINARY_DIR}/smFonts.png
)
