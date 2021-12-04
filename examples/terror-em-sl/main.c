#include "util/common.h"

#include "smAttribute.h"
#include "smIndexBuffer.h"
#include "smTexture.h"
#include "smUniform.h"
#include "smText.h"

#include "smInput.h"
#include "smDebug.h"
#include "smMesh.h"
#include "smModel.h"
#include "smSkinnedMesh.h"
#include "smSkinnedModel.h"
#include "smCamera.h"
#include "smShaderProgram.h"
#include "stage.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>

const int win_width = 800;
const int win_height = 600;
SDL_Window *window = NULL;
SDL_GLContext gc = NULL;

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    log_error("failed to initialize SDL: %s", SDL_GetError());
    exit(1);
  }

  window =
      SDL_CreateWindow("Terror em SL", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, win_width, win_height,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL |
                           SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_MOUSE_GRABBED);
  if (window == NULL) {
    log_error("failed to create SDL window: %s", SDL_GetError());
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  gc = SDL_GL_CreateContext(window);
  if (gc == NULL) {
    log_error("failed to create OpengGL ctx: %s", SDL_GetError());
    exit(1);
  }

  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    log_error("failed to initialize OpengGL ctx: %s", SDL_GetError());
    exit(1);
  }

  GLint n_attrs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n_attrs);
  log_info("GL Renderer: %s", glGetString(GL_RENDERER));
  log_info("GL Version: %s", glGetString(GL_VERSION));
  log_info("GL MAX_VERTEX_ATTRIBS: %d", n_attrs);

  SDL_SetRelativeMouseMode(SDL_TRUE);
  // Use v-sync
  SDL_GL_SetSwapInterval(1);


  shaders_init();
  text_init();
  input_init();
  debug_init();

  /* struct model_s *plane = model_new(); */
  /* if (!model_ctor(plane, "crate.obj", "crate.png")) { */
  /* exit(1); */
  /* } */

  struct stage_s *stage = stage_new();
  if (!stage_ctor(stage)) {
    exit(1);
  }
  /* struct skinned_model_s *woman = skinned_model_new(); */
  /* if (!skinned_model_ctor(woman, "goth.glb", "goth.png")) */
  /* exit(1); */


  Uint32 lastTicks = SDL_GetTicks();
  float dt = 0.0f;
  float ratio = (float)win_width / win_height;

  bool running = true;
  SDL_Event e;
  while (running) {

    input_before_do();
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = false;
      }
      input_do(&e);
    }

    glEnable(GL_DEPTH_TEST);
    /* glEnable(GL_BLEND); */
    /* glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   */
    glPointSize(3.0f);
    glViewport(0, 0, win_width, win_height);

    // 12/255, 12/255 12/255, 1.0f
    glClearColor(0.047, 0.047, 0.047, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    stage_do(stage, dt);
    stage_draw(stage, ratio);

    text_flush();
    glFinish(); // wait until queued GL commands have finished

    Uint32 newTicks = SDL_GetTicks();
    Uint32 ticksPassed = newTicks - lastTicks;
    dt = ((float)ticksPassed) * 0.001f; // milliseconds to seconds
    lastTicks = newTicks;

    SDL_GL_SwapWindow(window);
  }

  /* model_dtor(plane); */
  stage_dtor(stage);
  debug_tear_down();
  input_tear_down();
  text_tear_down();
  shaders_tear_down();
  SDL_GL_DeleteContext(gc);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
