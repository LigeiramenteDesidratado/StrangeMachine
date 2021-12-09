cmake_minimum_required(VERSION 3.5)

project(StrangeMachineEngine LANGUAGES C)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}" )
message(STATUS "C flags, Debug configuration: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "C flags, Release configuration: ${CMAKE_C_FLAGS_RELEASE}")


list(APPEND flags "-fPIC" "-Wall")
if(NOT WIN32)
  list(APPEND flags "-Wextra" "-Wpedantic")
endif()
message(STATUS "C flags: ${CMAKE_CXX_FLAGS}")


add_library(StrangeMachineEngine STATIC 
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
)

target_include_directories(StrangeMachineEngine PUBLIC "include" ${CMAKE_CURRENT_SOURCE_DIR})
