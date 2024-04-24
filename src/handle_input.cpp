#include "SDL_handler.h"
#include "components.h"
#include "entity_id.h"
#include "mPrint.h"
#include <SDL_events.h>
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

extern bool quit;
extern bool is_paused;
extern bool is_fullscreen;
extern bool is_gameover;
extern bool do_render_debug_panel;
extern entity_id player_id;
extern int color_index;
extern int num_color_indices;
extern int fullscreen_width;
extern int fullscreen_height;
extern int window_width;
extern int player_health;
extern int player_max_health;
extern int window_height;
extern int target_texture_width;
extern int target_texture_height;
extern int player_money;
extern SDL_Joystick *joystick;
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
extern unordered_map<string, SDL_Texture *> textures;
extern int gameover_count;

extern void init_game();
extern void generator_set_all_active_flip();
extern void toggle_fullscreen();
extern void spawn_knife();
extern void do_joystick();
// extern void cleanup_data_structures();
// extern void init_after_load_textures();
// extern void bg_init();
// extern void spawn_skull(const int x, const int y);
// extern void spawn_generator(enemy_type type, bool active, int group,
//                             int cooldown, int cooldown_reduction);
extern void screenshot();

void handle_joybuttondown() {
  // mPrint("joystick button down: " + to_string(e.jbutton.button));
  switch (e.jbutton.button) {
  case 0:
    is_pressed[SDLK_z] = true;
    break;
  default:
    break;
  }
}

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

void handle_joyhatmotion() {
  // mPrint("joystick hat motion: " + to_string(e.jhat.value));

  switch (e.jhat.value) {
  case SDL_HAT_CENTERED:
    is_pressed[SDLK_RIGHT] = false;
    is_pressed[SDLK_UP] = false;
    is_pressed[SDLK_DOWN] = false;
    is_pressed[SDLK_LEFT] = false;
    break;
  case SDL_HAT_UP:
    is_pressed[SDLK_UP] = true;
    break;
  case SDL_HAT_RIGHT:
    is_pressed[SDLK_RIGHT] = true;
    break;
  case SDL_HAT_RIGHTUP:
    is_pressed[SDLK_UP] = true;
    is_pressed[SDLK_RIGHT] = true;
    break;
  case SDL_HAT_DOWN:
    is_pressed[SDLK_DOWN] = true;
    break;
  case SDL_HAT_RIGHTDOWN:
    is_pressed[SDLK_DOWN] = true;
    is_pressed[SDLK_RIGHT] = true;
    break;
  case SDL_HAT_LEFT:
    is_pressed[SDLK_LEFT] = true;
    break;
  case SDL_HAT_LEFTUP:
    is_pressed[SDLK_LEFT] = true;
    is_pressed[SDLK_UP] = true;
    break;
  case SDL_HAT_LEFTDOWN:
    is_pressed[SDLK_DOWN] = true;
    is_pressed[SDLK_LEFT] = true;
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
    // mPrint("Escape pressed");
    is_paused = !is_paused;
    break;
  default:
    if (is_gameover) {
      // mPrint("gameover key press");
      gameover_count++;
      init_game();
    }
    break;
  }
}

void handle_joydeviceadded() {
  mPrint("joystick added");
  do_joystick();
}

void handle_joydeviceremoved() {
  mPrint("joystick removed");
  SDL_JoystickClose(joystick);
}

void handle_input() {
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      quit = true;
    } else if (e.type == SDL_KEYDOWN) {
      handle_keydown();
    } else if (e.type == SDL_KEYUP) {
      handle_keyup();
    } else if (e.type == SDL_JOYBUTTONDOWN) {
      handle_joybuttondown();
    } else if (e.type == SDL_JOYBUTTONUP) {
      handle_joybuttonup();
    } else if (e.type == SDL_JOYHATMOTION) {

      handle_joyhatmotion();
    } else if (e.type == SDL_JOYDEVICEADDED) {
      handle_joydeviceadded();
    } else if (e.type == SDL_JOYDEVICEREMOVED) {
      handle_joydeviceremoved();
    } else {
      mPrint("event type: " + to_string(e.type));
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
      transform.x -= 8;
      is_flipped[id] = true;
    } else if (is_pressed[SDLK_RIGHT]) {
      transform.x += 8;
      is_flipped[id] = false;
    }

    if (is_pressed[SDLK_UP] && is_pressed[SDLK_LSHIFT]) {
      transform.y -= 4;
    } else if (is_pressed[SDLK_DOWN] && is_pressed[SDLK_LSHIFT]) {
      transform.y += 4;
    } else if (is_pressed[SDLK_UP]) {
      transform.y -= 8;
    } else if (is_pressed[SDLK_DOWN]) {
      transform.y += 8;
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
