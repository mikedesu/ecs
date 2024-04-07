#include "entity_id.h"
#include "sprite_component.h"
#include "transform_component.h"
#include <unordered_map>

using std::unordered_map;

extern unordered_map<entity_id, bool> inputs;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<int, bool> is_pressed;
extern unordered_map<int, bool> is_flipped;

extern void spawn_knife();
// extern void generator_set_all_active_flip();

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

    // if (is_pressed[SDLK_g]) {
    //   generator_set_all_active_flip();
    // }

    transforms[id] = transform;
    sprites[id] = sprite;
  }
}
