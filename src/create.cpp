#include "SDL_handler.h"
// #include "gameconfig.h"
#include "mPrint.h"
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

// extern gameconfig config;
extern unordered_map<string, size_t> config;
// extern int window_width;
// extern int window_height;
extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern void cleanup_and_exit_with_failure_mprint(string msg);
extern void cleanup_and_exit_with_failure();

void create_window() {
  string window_title = "game";
  string keys[] = {"window_width", "window_height"};
  for (string k : keys) {
    if (config.find(k) == config.end()) {
      mPrint("Key not present in unordered_map config: " + k);
      cleanup_and_exit_with_failure();
    }
  }
  size_t window_width = config[keys[0]];
  size_t window_height = config[keys[1]];
  if (window_width == 0) {
    mPrint("Window width cannot be 0");
    cleanup_and_exit_with_failure();
  } else if (window_height == 0) {
    mPrint("Window height cannot be 0");
    cleanup_and_exit_with_failure();
  }
  window =
      SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
  if (window == nullptr) {
    mPrint("Failed to create window: " + string(SDL_GetError()));
    cleanup_and_exit_with_failure();
  }
}

void create_renderer() {
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr) {
    mPrint("Could not create renderer: " + string(SDL_GetError()));
    cleanup_and_exit_with_failure();
  }
}
