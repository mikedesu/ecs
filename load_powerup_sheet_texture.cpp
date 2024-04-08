#include "SDL_handler.h"

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

extern SDL_Renderer *renderer;
extern unordered_map<string, SDL_Texture *> textures;

extern void cleanup_and_exit_with_failure_mprint(string msg);

void load_powerup_sheet_texture() {
  string path = "img/powerup-sheet4x.png";
  SDL_Texture *t = IMG_LoadTexture(renderer, path.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         path);
  }
  textures["powerup"] = t;
}
