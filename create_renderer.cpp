#include "SDL_handler.h"
#include <string>
using std::string;
extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern void cleanup_and_exit_with_failure_mprint(string msg);

void create_renderer() {
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr) {
    cleanup_and_exit_with_failure_mprint("Could not create renderer: " +
                                         string(SDL_GetError()));
  }
}
