#include "SDL_handler.h"
#include "components.h"
#include "enemy_type.h"
#include "entity_id.h"
#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;

extern SDL_Renderer *renderer;
extern entity_id next_entity_id;
extern bool is_fullscreen;
extern int frame_count;
extern SDL_Rect target_texture_dest;
extern unordered_map<string, size_t> config;
extern SDL_Window *window;
extern unordered_map<enemy_type, int> enemies_killed;
extern unordered_map<entity_id, generator_component> generators;
extern unordered_map<entity_id, bool> is_generator;
extern vector<entity_id> entities;

entity_id get_next_entity_id() { return next_entity_id++; }

double distance(const int x1, const int y1, const int x2, const int y2) {
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
    int fullscreen_width, fullscreen_height;
    SDL_GetWindowSize(window, &fullscreen_width, &fullscreen_height);
    config["fullscreen_width"] = fullscreen_width;
    config["fullscreen_height"] = fullscreen_height;
    target_texture_dest.w = config["fullscreen_width"];
    target_texture_dest.h = config["fullscreen_height"];
  } else {
    target_texture_dest.w = config["window_width"];
    target_texture_dest.h = config["window_height"];
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

void screenshot() {
  int w, h;
  SDL_GetRendererOutputSize(renderer, &w, &h);
  const char *filepath = "screenshot.bmp";
  SDL_Surface *screenshot = SDL_CreateRGBSurface(
      0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
  SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
                       screenshot->pixels, screenshot->pitch);
  SDL_SaveBMP(screenshot, filepath);
  SDL_FreeSurface(screenshot);
}
