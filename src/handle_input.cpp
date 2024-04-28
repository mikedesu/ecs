#include "SDL_handler.h"
#include "components.h"
#include "entity_id.h"
#include "mPrint.h"
#include <SDL_events.h>
#include <map>
#include <string>
#include <unordered_map>

using std::map;
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
extern map<entity_id, sprite_component> sprites;
// extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<int, bool> is_pressed;
extern unordered_map<int, bool> is_flipped;
extern unordered_map<int, bool> is_pressed;
extern unordered_map<entity_id, sprite_component> sprite_components;
extern unordered_map<string, SDL_Texture *> textures;
extern int gameover_count;
extern int current_player_speed;
extern int default_player_speed;
// extern int current_knife_speed;
// extern int default_knife_speed;

extern void init_game();
extern void generator_set_all_active_flip();
extern void toggle_fullscreen();
extern void spawn_knife();
// extern void spawn_knives();
extern void do_joystick();
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

void handle_joyaxismotion() {
  mPrint("handle_joyaxismotion");

  if (e.jaxis.which == 0) {
    if (e.jaxis.axis == 0) {
      // mPrint("");
      if (e.jaxis.value < -8000) {
        // left
        is_pressed[SDLK_LEFT] = true;
      } else if (e.jaxis.value > 8000) {
        // right
        is_pressed[SDLK_RIGHT] = true;
      } else {
        // no left/right press
        is_pressed[SDLK_LEFT] = false;
        is_pressed[SDLK_RIGHT] = false;
      }
    } else if (e.jaxis.axis == 1) {
      // mPrint("");
      if (e.jaxis.value < -8000) {
        // up
        is_pressed[SDLK_UP] = true;
      } else if (e.jaxis.value > 8000) {
        is_pressed[SDLK_DOWN] = true;
        // down
      } else {
        // no up/down press
        is_pressed[SDLK_UP] = false;
        is_pressed[SDLK_DOWN] = false;
      }
    }
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
    // case SDL_JOYBUTTONDOWN:
    //   handle_joybuttondown();
    //   break;
    // case SDL_JOYBUTTONUP:
    //   handle_joybuttonup();
    //   break;
    // case SDL_JOYHATMOTION:
    //   handle_joyhatmotion();
    //   break;
    // case SDL_JOYDEVICEADDED:
    //   handle_joydeviceadded();
    //   break;
    // case SDL_JOYDEVICEREMOVED:
    //   handle_joydeviceremoved();
    //   break;
    // case SDL_JOYAXISMOTION:
    //   handle_joyaxismotion();
    //   break;
    case SDL_MOUSEMOTION:
      break;

    default:
      // mPrint("event type: " + to_string(e.type));
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
      // spawn_knives();
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
