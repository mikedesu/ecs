#include "SDL_handler.h"
#include "entity_id.h"
#include "sprite_component.h"
#include "transform_component.h"
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

extern bool quit;
extern bool is_fullscreen;
extern bool do_render_debug_panel;
extern int fullscreen_width;
extern int fullscreen_height;
extern int window_width;
extern int window_height;
extern int target_texture_width;
extern int target_texture_height;
extern int player_money;
extern SDL_Event e;
extern SDL_Rect target_texture_dest;
extern SDL_Window *window;
extern unordered_map<entity_id, bool> inputs;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<int, bool> is_pressed;
extern unordered_map<int, bool> is_flipped;
extern unordered_map<int, bool> is_pressed;
extern unordered_map<entity_id, sprite_component> sprite_components;

extern void generator_set_all_active_flip();
extern void toggle_fullscreen();
extern void spawn_powerup();
extern void spawn_knife();

void handle_keyup() {
  switch (e.key.keysym.sym) {
  case SDLK_LEFT:
  case SDLK_RIGHT:
  case SDLK_UP:
  case SDLK_DOWN:
  case SDLK_a:
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
  case SDLK_u:

    if (player_money > 1) {
      player_money -= 1;
      spawn_powerup();
    }
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

void handle_input_component() {
  for (auto kv : inputs) {
    entity_id id = kv.first;
    transform_component transform = transforms[id];
    sprite_component sprite = sprites[id];

    if (is_pressed[SDLK_LEFT] && is_pressed[SDLK_LSHIFT]) {
      transform.x -= 8;
      is_flipped[id] = true;
    } else if (is_pressed[SDLK_RIGHT] && is_pressed[SDLK_LSHIFT]) {
      transform.x += 8;
      is_flipped[id] = false;
    } else if (is_pressed[SDLK_LEFT]) {
      transform.x -= 4;
      is_flipped[id] = true;
    } else if (is_pressed[SDLK_RIGHT]) {
      transform.x += 4;
      is_flipped[id] = false;
    }

    if (is_pressed[SDLK_UP] && is_pressed[SDLK_LSHIFT]) {
      transform.y -= 8;
    } else if (is_pressed[SDLK_DOWN] && is_pressed[SDLK_LSHIFT]) {
      transform.y += 8;
    } else if (is_pressed[SDLK_UP]) {
      transform.y -= 4;
    } else if (is_pressed[SDLK_DOWN]) {
      transform.y += 4;
    }

    if (is_pressed[SDLK_a]) {
      spawn_knife();
      sprite.current_clip = 1;
      sprite.src.x = sprite.current_clip * sprite.src.w;
    } else {
      sprite.current_clip = 0;
      sprite.src.x = sprite.current_clip * sprite.src.w;
    }

    transforms[id] = transform;
    sprites[id] = sprite;
  }
}
