cmake_minimum_required(VERSION 3.21.4)

project(StrangeMachineEngine LANGUAGES C CXX)

find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

message(STATUS "${PROJECT_NAME}:")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}" )
message(STATUS "Debug cfg: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "Release cfg: ${CMAKE_C_FLAGS_RELEASE}")

set(SM_SOURCE_FILES)

list(APPEND SM_SOURCE_FILES

  core/smApplication.c
  core/smWindow.c
  core/smPool.c
  core/smHandle.c
  core/smStackLayer.c
  core/smLayer.c
  core/smKeyCode.c
  core/smTimer.c
  core/smTime.c

  core/thread/smThread.c
  core/thread/synchronization/smMutex.c

  core/util/smString.c
  core/util/smFile.c
  core/util/smDir.c

  renderer/smRenderer2D.c
  renderer/smDevice.c

  renderer/smRenderer3D.c

  renderer/api/GL21/smVertexBuffer.c
  renderer/api/GL21/smIndexBuffer.c
  renderer/api/GL21/smShader.c
  renderer/api/GL21/smDraw.c
  renderer/api/GL21/smTexture.c

  cimgui/smCimgui.c

  resource/smResource.c
  resource/smTextureResource.c

  scene/smScene.c
  scene/smComponents.c
  scene/smSystem.c

  event/smEvent.c

  smLoader.c
  #  smAttribute.c
  smCamera.c
  #  smClip.c
  #  smController.c
  #  smCrossFadeTarget.c
  #  smDebug.c
  #  smGLTFLoader.c
  #  smIndexBuffer.c
  smInput.c
  #  smMesh.c
  #  smModel.c
  #  smOBJLoader.c
  #  smPhysics.c
  #  smPose.c
  #  smRearrangeBones.c
  #  smShader.c
  #  smShaderProgram.c
  # smShapes.c
  #  smSkeleton.c
  #  smSkinnedMesh.c
  #  smSkinnedModel.c
  #  smText.c
  #  smTexture.c
  #  smTrack.c
  #  smTransformTrack.c
  #  smSkyBox.c
  #  smCollision.c

  math/smTransform.c
  vendor/logc/src/log.c
  vendor/gladGL21/glad.c
  vendor/cimgui/imgui/backends/imgui_impl_opengl2.cpp
  vendor/cimgui/imgui/backends/imgui_impl_sdl.cpp
  )

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  list(APPEND SM_SOURCE_FILES
    core/smMem.c
    renderer/api/GL21/smGLUtil.c
    )
endif()

add_library(${PROJECT_NAME} STATIC ${SM_SOURCE_FILES})
target_precompile_headers(${PROJECT_NAME} PUBLIC smpch.h)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PRIVATE LOG_USE_COLOR)
target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Debug>:SM_DEBUG>")


# target_compile_flags(${PROJECT_NAME} PRIVATE SM_FLAGS)


# cimgui library
option(IMGUI_STATIC "Static build" ON)
#  add glad to the imgui library
add_subdirectory(vendor/cimgui)

# My math library
#  add_subdirectory(math_deprec)

# cglm library
option(CGLM_STATIC "Static build" ON)
add_subdirectory(vendor/cglm)


# dmon
#  add_subdirectory(vendor/dmon)

target_include_directories(${PROJECT_NAME} PRIVATE "include" ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC cglm PRIVATE assimp stdc++ cimgui SDL2 m GL)

# Copy content to build folder:
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  # Copy shader folder
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/glsl ${CMAKE_CURRENT_BINARY_DIR}/glsl
  # Copy font atlas
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/smFonts.png ${CMAKE_CURRENT_BINARY_DIR}/smFonts.png
)
