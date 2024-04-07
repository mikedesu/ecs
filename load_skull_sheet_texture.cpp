#include "SDL_handler.h"
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

extern SDL_Renderer *renderer;
extern unordered_map<string, SDL_Texture *> textures;

extern string skullsheet_filepath;

extern void cleanup_and_exit_with_failure_mprint(string msg);

void load_skull_sheet_texture() {
  SDL_Texture *t = IMG_LoadTexture(renderer, skullsheet_filepath.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         skullsheet_filepath);
  }
  textures["skull"] = t;
}
