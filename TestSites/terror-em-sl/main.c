#include "cglm/quat.h"
#include "smAttribute.h"
#include "smIndexBuffer.h"
#include "smText.h"
#include "smTexture.h"
#include "smUniform.h"

#include "smAttribute.h"
#include "smCamera.h"
#include "smDebug.h"
#include "smInput.h"
#include "smMem.h"
#include "smMesh.h"
#include "smModel.h"
#include "smShaderProgram.h"
#include "smSkinnedMesh.h"
#include "smSkinnedModel.h"
#include "stage.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "vendor/cimgui/cimgui.h"
#include "vendor/cimgui/generator/output/cimgui_impl.h"

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
    printf("failed to initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  // create a full screen window
  /* window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, */
  /* SDL_WINDOWPOS_UNDEFINED, win_width, win_height, */
  /* SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN); */

  window = SDL_CreateWindow("Terror em SL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_height,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_MOUSE_GRABBED);
  if (window == NULL) {
    printf("failed to create SDL window: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  gc = SDL_GL_CreateContext(window);
  if (gc == NULL) {
    printf("failed to create OpengGL ctx: %s\n", SDL_GetError());
    exit(1);
  }

  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    printf("failed to initialize OpengGL ctx: %s\n", SDL_GetError());
    exit(1);
  }

  GLint n_attrs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n_attrs);
  printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
  printf("GL Version: %s\n", glGetString(GL_VERSION));
  printf("GL MAX_VERTEX_ATTRIBS: %d\n", n_attrs);

  /* SDL_SetRelativeMouseMode(SDL_TRUE); */
  // Use v-sync
  SDL_GL_SetSwapInterval(1);

  ImGuiContext *imCtx = igCreateContext(NULL);

  ImGuiIO *io = igGetIO();
  (void)io;

  ImGui_ImplSDL2_InitForOpenGL(window, gc);
  ImGui_ImplOpenGL2_Init();

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
  bool show_demo_window = true;
  SDL_Event e;
  while (running) {

    /* input_before_do(); */
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = false;
      }
      ImGui_ImplSDL2_ProcessEvent(&e);
      input_do(&e);
    }

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();

    if (show_demo_window)
      igShowDemoWindow(&show_demo_window);

    glEnable(GL_DEPTH_TEST);
    /* glEnable(GL_BLEND); */
    /* glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   */
    glPointSize(3.0f);
    glViewport(0, 0, win_width, win_height);

    // 12/255, 12/255 12/255, 1.0f
    glClearColor(0.047f, 0.047f, 0.047f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    igRender();

    stage_do(stage, dt);
    stage_draw(stage, ratio);

    text_flush();
    glFinish(); // wait until queued GL commands have finished

    Uint32 newTicks = SDL_GetTicks();
    Uint32 ticksPassed = newTicks - lastTicks;
    dt = ((float)ticksPassed) * 0.001f; // milliseconds to seconds
    lastTicks = newTicks;

    ImGui_ImplOpenGL2_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);
    stats.frames++;
  }

  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  igDestroyContext(imCtx);
  /* model_dtor(plane); */
  stage_dtor(stage);
  debug_tear_down();
  input_tear_down();
  text_tear_down();
  shaders_tear_down();
  SDL_GL_DeleteContext(gc);
  SDL_DestroyWindow(window);
  SDL_Quit();

  /* __smmem_print(); */

  return 0;
}
