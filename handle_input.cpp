#include "SDL_handler.h"
#include "mPrint.h"
#include <string>
#include <unordered_map>

using std::string;
using std::to_string;
using std::unordered_map;

extern SDL_Window *window;
extern bool quit;
extern bool is_fullscreen;
extern bool do_render_debug_panel;
extern SDL_Event e;
extern unordered_map<int, bool> is_pressed;
extern int fullscreen_width;
extern int fullscreen_height;
extern int window_width;
extern int window_height;
extern int target_texture_width;
extern int target_texture_height;
extern SDL_Rect target_texture_dest;

extern void generator_set_all_active_flip();
extern void toggle_fullscreen();

void handle_keyup() {
  switch (e.key.keysym.sym) {
  case SDLK_LEFT:
  case SDLK_RIGHT:
  case SDLK_UP:
  case SDLK_DOWN:
  case SDLK_a:
  // case SDLK_g:
  case SDLK_x:
  case SDLK_z:
  case SDLK_LSHIFT:
  case SDLK_RSHIFT:
    is_pressed[e.key.keysym.sym] = false;
    break;
  default:
    break;
  }
}

void handle_keydown() {
  switch (e.key.keysym.sym) {
  case SDLK_LEFT:
  case SDLK_RIGHT:
  case SDLK_UP:
  case SDLK_DOWN:
  case SDLK_a:
  // case SDLK_g:
  case SDLK_x:
  case SDLK_z:
  case SDLK_LSHIFT:
  case SDLK_RSHIFT:
    is_pressed[e.key.keysym.sym] = true;
    break;
  case SDLK_q:
    quit = true;
    break;
  case SDLK_d:
    do_render_debug_panel = !do_render_debug_panel;
    break;
  case SDLK_g:
    generator_set_all_active_flip();
    break;
  case SDLK_f:

    toggle_fullscreen();

    break;

  default:
    break;
  }
}

void handle_input() {
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      quit = true;
    } else if (e.type == SDL_KEYDOWN) {
      handle_keydown();
    } else if (e.type == SDL_KEYUP) {
      handle_keyup();
    }
  }
}
