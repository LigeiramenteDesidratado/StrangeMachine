#include "smpch.h"

#include "cimgui/smCimgui.h"

#include "core/smCore.h"
#include "core/smWindow.h"
#include "util/colors.h"

#include <SDL2/SDL.h>

static void sm__cimgui_on_attach(void *user_data);
static void sm__cimgui_on_detach(void *user_data);
static b8 sm__cimgui_on_event(event_s *event, void *user_data);
static void sm__set_dark_theme_colors();

cimgui_s *cimgui_new(void) {

  return layer_new();
}

b8 cimgui_ctor(cimgui_s *cimgui, struct window_s *window) {

  SM_ASSERT(cimgui);

  return layer_ctor(cimgui, "CImGui", window, sm__cimgui_on_attach, sm__cimgui_on_detach, NULL, NULL,
                    sm__cimgui_on_event);
}

void cimgui_dtor(cimgui_s *cimgui) {

  SM_ASSERT(cimgui);

  layer_dtor(cimgui);
}

static void sm__cimgui_on_attach(void *user_data) {

  SM_ASSERT(user_data);

  struct window_s *window = user_data;

  igCreateContext(NULL);

  SDL_Window *raw_win = window_get_window_raw(window);
  void *raw_ctx = window_get_context_raw(window);

  sm__set_dark_theme_colors();

  ImGuiIO *io = igGetIO();
  io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  // ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifdef IMGUI_HAS_DOCK
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
  io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
#endif

  ImGui_ImplSDL2_InitForOpenGL(raw_win, raw_ctx);
  ImGui_ImplOpenGL2_Init();
}

static void sm__cimgui_on_detach(void *user_data) {

  SM_ASSERT(user_data);

  cimgui_s *cimgui = (cimgui_s *)user_data;

  SM_ASSERT(cimgui);

  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplSDL2_Shutdown();

  ImGuiContext *im_ctx = igGetCurrentContext();
  igDestroyContext(im_ctx);
}

static b8 sm__cimgui_on_event(event_s *event, void *user_data) {

  SM_ASSERT(user_data);
  SM_ASSERT(event);

  ImGuiIO *io = igGetIO();

  event->handled |= (SM_MASK_CHK(event->category, SM_CATEGORY_MOUSE) != 0) & io->WantCaptureMouse;
  event->handled |= (SM_MASK_CHK(event->category, SM_CATEGORY_KEYBOARD) != 0) & io->WantCaptureKeyboard;
  event->handled |= io->WantCaptureKeyboard;

  return event->handled;
}

void cimgui_begin(cimgui_s *cimgui) {

  SM_ASSERT(cimgui);

  ImGui_ImplOpenGL2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  igNewFrame();
}

void cimgui_end(cimgui_s *cimgui) {

  SM_ASSERT(cimgui);

  igRender();
  ImGui_ImplOpenGL2_RenderDrawData(igGetDrawData());
}

#define VEC4_TO_IM_VEC4(VEC4) ((ImVec4){VEC4.data[0], VEC4.data[1], VEC4.data[2], VEC4.data[3]})

#include "smFontGo.embed"

static void sm__set_dark_theme_colors() {

  ImGuiStyle *style = igGetStyle();

  ImVec4 *colors = style->Colors;

  ImGuiIO *io = igGetIO();

  ImFontConfig config = {0};
  config.EllipsisChar = -1;
  config.FontDataOwnedByAtlas = false;
  config.RasterizerMultiply = 1.0f;
  config.FontBuilderFlags = 0;
  config.MergeMode = false;
  config.GlyphMinAdvanceX = 0.0f;
  config.GlyphMaxAdvanceX = FLT_MAX;
  config.OversampleH = 3;
  config.OversampleV = 1;

  ImFont *gomono =
      ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, (void *)Go_Regular_ttf, Go_Regular_ttf_len, 16.0f, &config, NULL);
  io->FontDefault = gomono;

  colors[ImGuiCol_WindowBg] = VEC4_TO_IM_VEC4(SM_BACKGROUND_COLOR);
  colors[ImGuiCol_Text] = VEC4_TO_IM_VEC4(SM_FOREGROUND_COLOR);

  /* Separator */
  colors[ImGuiCol_SeparatorHovered] = VEC4_TO_IM_VEC4(SM_MAIN_COLOR_1);
  colors[ImGuiCol_SeparatorActive] = VEC4_TO_IM_VEC4(SM_MAIN_COLOR_2);

  /* Resize Grip */
  colors[ImGuiCol_ResizeGrip] = VEC4_TO_IM_VEC4(SM_MAIN_COLOR_0);
  colors[ImGuiCol_ResizeGripHovered] = VEC4_TO_IM_VEC4(SM_MAIN_COLOR_1);
  colors[ImGuiCol_ResizeGripActive] = VEC4_TO_IM_VEC4(SM_MAIN_COLOR_2);

  // Headers
  colors[ImGuiCol_Header] = (ImVec4){0.2f, 0.205f, 0.21f, 1.0f};
  colors[ImGuiCol_HeaderHovered] = (ImVec4){0.3f, 0.305f, 0.31f, 1.0f};
  colors[ImGuiCol_HeaderActive] = (ImVec4){0.15f, 0.1505f, 0.151f, 1.0f};

  // Buttons
  colors[ImGuiCol_Button] = (ImVec4){0.2f, 0.205f, 0.21f, 1.0f};
  colors[ImGuiCol_ButtonHovered] = (ImVec4){0.3f, 0.305f, 0.31f, 1.0f};
  colors[ImGuiCol_ButtonActive] = (ImVec4){0.15f, 0.1505f, 0.151f, 1.0f};

  // Frame BG
  colors[ImGuiCol_FrameBg] = (ImVec4){0.2f, 0.205f, 0.21f, 1.0f};
  colors[ImGuiCol_FrameBgHovered] = (ImVec4){0.3f, 0.305f, 0.31f, 1.0f};
  colors[ImGuiCol_FrameBgActive] = (ImVec4){0.15f, 0.1505f, 0.151f, 1.0f};

  colors[ImGuiCol_CheckMark] = VEC4_TO_IM_VEC4(SM_MAIN_COLOR_1);
  colors[ImGuiCol_SliderGrab] = VEC4_TO_IM_VEC4(SM_MAIN_COLOR_1);
  colors[ImGuiCol_SliderGrabActive] = VEC4_TO_IM_VEC4(SM_MAIN_COLOR_2);

  // Tabs
  colors[ImGuiCol_Tab] = (ImVec4){0.15f, 0.1505f, 0.151f, 1.0f};
  colors[ImGuiCol_TabHovered] = (ImVec4){0.38f, 0.3805f, 0.381f, 1.0f};
  colors[ImGuiCol_TabActive] = (ImVec4){0.28f, 0.2805f, 0.281f, 1.0f};
  colors[ImGuiCol_TabUnfocused] = (ImVec4){0.15f, 0.1505f, 0.151f, 1.0f};
  colors[ImGuiCol_TabUnfocusedActive] = (ImVec4){0.2f, 0.205f, 0.21f, 1.0f};

  // Title
  colors[ImGuiCol_TitleBg] = (ImVec4){0.15f, 0.1505f, 0.151f, 1.0f};
  colors[ImGuiCol_TitleBgActive] = (ImVec4){0.15f, 0.1505f, 0.151f, 1.0f};
  colors[ImGuiCol_TitleBgCollapsed] = (ImVec4){0.15f, 0.1505f, 0.151f, 1.0f};
}
