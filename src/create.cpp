#include "SDL_handler.h"
#include "gameconfig.h"
#include "mPrint.h"
#include <string>
extern gameconfig config;
extern int window_width;
extern int window_height;
extern SDL_Window *window;
extern SDL_Renderer *renderer;

using std::string;

extern void cleanup_and_exit_with_failure_mprint(string msg);
extern void cleanup_and_exit_with_failure();

void create_window() {

  if (config.window_width == 0 || config.window_height == 0) {
    mPrint("Window width and height must be set");
    cleanup_and_exit_with_failure();
  }

  window = SDL_CreateWindow(
      "SDL2 Displaying Image", SDL_WINDOWPOS_UNDEFINED,
      // SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
      SDL_WINDOWPOS_UNDEFINED, config.window_width, config.window_height, 0);
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
