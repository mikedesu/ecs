
#include "SDL_handler.h"
#include "mPrint.h"
#include <string>

using std::string;

extern SDL_Renderer *renderer;
extern SDL_Texture *target_texture;
extern int target_texture_width;
extern int target_texture_height;
extern int result;

extern void cleanup_and_exit_with_failure_mprint(string message);

int init_target_texture() {
  target_texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      target_texture_width, target_texture_height);
  if (target_texture == nullptr) {
    mPrint("Failed to create target texture!");
    return 0;
  }
  return 1;
}

void handle_init_target_texture() {
  result = init_target_texture();
  if (!result) {
    cleanup_and_exit_with_failure_mprint("Failed to init target texture");
  }
}
