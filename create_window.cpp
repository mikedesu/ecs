#include "SDL_handler.h"
#include <string>
extern int window_width;
extern int window_height;
extern SDL_Window *window;

using std::string;

extern void cleanup_and_exit_with_failure_mprint(string msg);

void create_window() {
  window =
      SDL_CreateWindow("SDL2 Displaying Image", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
  if (window == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to create window: " +
                                         string(SDL_GetError()));
  }
}
