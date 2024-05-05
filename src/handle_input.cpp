#include "SDL_handler.h"
#include "components.h"
#include "entity_id.h"
// #include "mPrint.h"
#include <SDL_events.h>
#include <map>
#include <string>
#include <unordered_map>

using std::map;
using std::string;
using std::unordered_map;

extern bool quit;
extern bool is_paused;
extern bool is_gameover;
extern bool do_render_debug_panel;
extern SDL_Event e;
extern map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, bool> inputs;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<int, bool> is_flipped;
extern unordered_map<int, bool> is_pressed;
extern int gameover_count;
extern int current_player_speed;

extern void init_game();
extern void generator_set_all_active_flip();
extern void toggle_fullscreen();
extern void spawn_knife();
extern void screenshot();

void handle_joybuttonup() {
  // mPrint("joystick button up: " + to_string(e.jbutton.button));
  switch (e.jbutton.button) {
  case 0:
    is_pressed[SDLK_z] = false;
    break;
  default:
    break;
  }
}

void handle_keyup() {
  switch (e.key.keysym.sym) {
  case SDLK_LEFT:
  case SDLK_RIGHT:
  case SDLK_UP:
  case SDLK_DOWN:
  case SDLK_z:
  case SDLK_x:
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
  case SDLK_z:
  case SDLK_x:
  case SDLK_LSHIFT:
  case SDLK_RSHIFT:
    is_pressed[e.key.keysym.sym] = true;
    break;
  case SDLK_s:
    screenshot();
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
  case SDLK_ESCAPE:
    is_paused = !is_paused;
    break;
  default:
    if (is_gameover) {
      gameover_count++;
      init_game();
    }
    break;
  }
}

void handle_input() {
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      quit = true;
      break;
    case SDL_KEYDOWN:
      handle_keydown();
      break;
    case SDL_KEYUP:
      handle_keyup();
      break;
    default:
      break;
    }
  }
}

void handle_input_component() {
  for (auto kv : inputs) {
    entity_id id = kv.first;
    transform_component transform = transforms[id];
    sprite_component sprite = sprites[id];

    if (is_pressed[SDLK_LEFT] && is_pressed[SDLK_LSHIFT]) {
      transform.x -= 4;
      is_flipped[id] = true;
    } else if (is_pressed[SDLK_RIGHT] && is_pressed[SDLK_LSHIFT]) {
      transform.x += 4;
      is_flipped[id] = false;
    } else if (is_pressed[SDLK_LEFT]) {
      transform.x -= current_player_speed;
      is_flipped[id] = true;
    } else if (is_pressed[SDLK_RIGHT]) {
      transform.x += current_player_speed;
      is_flipped[id] = false;
    }

    if (is_pressed[SDLK_UP] && is_pressed[SDLK_LSHIFT]) {
      transform.y -= 4;
    } else if (is_pressed[SDLK_DOWN] && is_pressed[SDLK_LSHIFT]) {
      transform.y += 4;
    } else if (is_pressed[SDLK_UP]) {
      transform.y -= current_player_speed;
    } else if (is_pressed[SDLK_DOWN]) {
      transform.y += current_player_speed;
    }

    if (is_pressed[SDLK_z]) {
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
