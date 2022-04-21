#include <SDL2/SDL.h>

#include "vendor/gladGL21/glad.h"

#include "core/smCore.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "vendor/cimgui/cimgui.h"
#include "vendor/cimgui/generator/output/cimgui_impl.h"

SDL_Window *raw_window = NULL;
SDL_GLContext raw_context = NULL;
uint32_t width, height;
bool vsync;

bool window_init2(const char *name, uint32_t width, uint32_t height) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("failed to initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  // create a full screen window
  /* window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, */
  /* SDL_WINDOWPOS_UNDEFINED, win_width, win_height, */
  /* SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN); */

  raw_window =
      SDL_CreateWindow("Terror em SL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_MOUSE_GRABBED);
  if (raw_window == NULL) {
    printf("failed to create SDL window: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  raw_context = SDL_GL_CreateContext(raw_window);
  if (raw_context == NULL) {
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

  return true;
}
bool window_init(const char *name, uint32_t width, uint32_t height) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SM_CORE_LOG_ERROR("SDL_Init Error: %s", SDL_GetError());
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  raw_window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

  if (raw_window == NULL) {
    SM_CORE_LOG_ERROR("SDL_CreateWindow Error: %s", SDL_GetError());
    return false;
  }

  raw_context = SDL_GL_CreateContext(raw_window);
  if (raw_context == NULL) {
    SM_CORE_LOG_ERROR("SDL_GL_CreateContext Error: %s", SDL_GetError());
    return false;
  }

  /* TODO: LOAD OPENGL FUNCTIONS PROPERLY */
  if (SDL_GL_MakeCurrent(raw_window, raw_context) != 0) {
    SM_CORE_LOG_ERROR("SDL_GL_MakeCurrent Error: %s", SDL_GetError());
    return false;
  }

  /* bool err = gladLoadGL() == 0; */
  /* if (err) { */
  /*   SM_CORE_LOG_ERROR("gladLoadGL Error: %s", SDL_GetError()); */
  /*   return false; */
  /* } */
  /* gladLoadGL(); */
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    SM_CORE_LOG_ERROR("failed to initialize OpengGL ctx: %s\n", SDL_GetError());
    return false;
  }

  GLint n_attrs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n_attrs);
  SM_CORE_LOG_DEBUG("GL Renderer: %s", glGetString(GL_RENDERER));
  SM_CORE_LOG_DEBUG("GL Version: %s", glGetString(GL_VERSION));
  SM_CORE_LOG_DEBUG("GL MAX_VERTEX_ATTRIBS: %d", n_attrs);

  // vsync enable by default
  vsync = true;
  if (SDL_GL_SetSwapInterval(vsync) < 0) {
    SM_CORE_LOG_WARN("SDL_GL_SetSwapInterval Error: %s", SDL_GetError());
  }

  width = width;
  height = height;

  SM_LOG_INFO("Window created: %s", name);

  return true;
}

void window_deinit() {

  SDL_GL_DeleteContext(raw_context);
  SDL_DestroyWindow(raw_window);
  SDL_Quit();
}

static void SimpleOverlay(bool *p_open) {
  static int corner = 0;
  ImGuiIO *io = igGetIO();
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                  ImGuiWindowFlags_NoNav;
  if (corner != -1) {
    const float PAD = 10.0f;
    const ImGuiViewport *viewport = igGetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
    igSetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    window_flags |= ImGuiWindowFlags_NoMove;
  }
  igSetNextWindowBgAlpha(0.35f); // Transparent background
  if (igBegin("Example: Simple overlay", p_open, window_flags)) {
    igText("Quad Count: %d", 22);
    igText("Draw Calls: %d", 1);
    /* if (igIsMousePosValid()) */
    /* igText("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y); */
    /* else */
    /* igText("Mouse Position: <invalid>"); */
  }
  igEnd();
}

int main(int argc, char *argv[]) {

  window_init2("Debug", 800, 600);
  /* window_init2("Debug", 800, 600); */

  bool running = true;

  igCreateContext(NULL);

  ImGuiIO *io = igGetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  // ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifdef IMGUI_HAS_DOCK
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
  io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
#endif

  ImGui_ImplSDL2_InitForOpenGL(raw_window, raw_context);
  ImGui_ImplOpenGL2_Init();

  while (running) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      }
    }

    /* SDL_GL_MakeCurrent(raw_window, raw_context); */
    /* glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y); */
    glClearColor(0.047f, 0.047f, 0.047f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();

    static bool show_demo_window = true;
    static bool show_simple_overlay = true;
    if (show_demo_window)
      igShowDemoWindow(&show_demo_window);
    if (show_simple_overlay)
      SimpleOverlay(&show_simple_overlay);

    igRender();
    ImGui_ImplOpenGL2_RenderDrawData(igGetDrawData());

#ifdef IMGUI_HAS_DOCK
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
      SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
      igUpdatePlatformWindows();
      igRenderPlatformWindowsDefault(NULL, NULL);
      SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
#endif

    SDL_GL_SwapWindow(raw_window);
  }

  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  /* igDestroyContext(imCtx); */

  window_deinit();
  return 0;
}
