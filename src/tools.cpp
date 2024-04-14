#include "SDL_handler.h"
#include "components.h"
#include "enemy_type.h"
#include "entity_id.h"
#include <unordered_map>
#include <vector>

using std::unordered_map;
using std::vector;

extern entity_id next_entity_id;
extern bool is_fullscreen;
extern int fullscreen_width;
extern int fullscreen_height;
extern int window_width;
extern int window_height;
extern int frame_count;
extern SDL_Rect target_texture_dest;
extern SDL_Window *window;
extern unordered_map<enemy_type, int> enemies_killed;
extern unordered_map<entity_id, generator_component> generators;
extern unordered_map<entity_id, bool> is_generator;
extern vector<entity_id> entities;

entity_id get_next_entity_id() { return next_entity_id++; }

double distance(int x1, int y1, int x2, int y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

size_t get_num_enemies_killed() {
  size_t total = 0;
  for (auto kv : enemies_killed) {
    total += kv.second;
  }
  return total;
}

void toggle_fullscreen() {
  is_fullscreen = !is_fullscreen;
  SDL_SetWindowFullscreen(window,
                          is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  if (is_fullscreen) {
    SDL_GetWindowSize(window, &fullscreen_width, &fullscreen_height);
    target_texture_dest.w = fullscreen_width;
    target_texture_dest.h = fullscreen_height;
  } else {
    target_texture_dest.w = window_width;
    target_texture_dest.h = window_height;
  }
}

void generator_set_active(entity_id id, bool active) {
  if (generators.find(id) != generators.end()) {
    generator_component generator = generators[id];
    generator.active = active;
    generators[id] = generator;
  }
}

void generator_set_active_flip(entity_id id) {
  // mPrint("generator_set_active_flip");
  if (is_generator[id]) {
    generator_component generator = generators[id];
    generator.active = !generator.active;
    generators[id] = generator;
  }
}

void generator_set_all_active_flip() {
  for (auto id : entities) {
    generator_set_active_flip(id);
  }
}

void generator_set_all_active() {
  for (auto id : entities) {
    generator_set_active(id, true);
  }
}

void generator_set_all_inactive() {
  for (auto id : entities) {
    generator_set_active(id, false);
  }
}
