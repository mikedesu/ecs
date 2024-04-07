#include "SDL_handler.h"

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

extern SDL_Renderer *renderer;
extern unordered_map<string, SDL_Texture *> textures;
extern string coin_sheet_filepath;

extern void cleanup_and_exit_with_failure_mprint(string msg);

void load_coin_sheet_texture() {
  SDL_Texture *t = IMG_LoadTexture(renderer, coin_sheet_filepath.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         coin_sheet_filepath);
  }
  textures["coin"] = t;
}
