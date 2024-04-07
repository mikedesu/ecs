#include "SDL_handler.h"
#include "entity_id.h"
#include "mPrint.h"
#include "sprite_component.h"
#include "transform_component.h"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

using std::exit;
using std::remove;
using std::string;
using std::unordered_map;
using std::vector;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *gFont;
extern SDL_Texture *debug_texture;
extern SDL_Texture *debug_bg_texture;
extern SDL_Texture *target_texture;

extern unordered_map<string, SDL_Texture *> textures;

extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, bool> inputs;
extern unordered_map<entity_id, bool> is_rotating;
extern unordered_map<entity_id, bool> is_collidable;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_marked_for_deletion;

extern vector<entity_id> entities;
extern vector<entity_id> entities_marked_for_deletion_tmp;

extern unordered_map<int, bool> is_pressed;

void cleanup_textures();
void cleanup_and_exit_with_failure();
void cleanup_and_exit_with_failure_mprint(string message);
void cleanup_entities_marked_for_deletion();

void cleanup_textures() {
  for (auto kv : textures) {
    SDL_DestroyTexture(kv.second);
  }
  textures.clear();
}

void cleanup() {
  mPrint("cleaning up");
  sprites.clear();
  transforms.clear();
  inputs.clear();
  entities.clear();
  is_pressed.clear();
  is_rotating.clear();
  is_collidable.clear();
  is_enemy.clear();
  is_knife.clear();
  is_marked_for_deletion.clear();
  cleanup_textures();
  SDL_DestroyTexture(debug_texture);
  SDL_DestroyTexture(debug_bg_texture);
  SDL_DestroyTexture(target_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(gFont);
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

void cleanup_and_exit_with_failure() {
  cleanup();
  exit(EXIT_FAILURE);
}

void cleanup_and_exit_with_failure_mprint(string message) {
  mPrint(message);
  cleanup();
  exit(EXIT_FAILURE);
}

void cleanup_entities_marked_for_deletion() {
  for (auto kv : is_marked_for_deletion) {
    entity_id id = kv.first;
    if (kv.second) {
      sprites.erase(id);
      transforms.erase(id);
      inputs.erase(id);
      is_rotating.erase(id);
      is_collidable.erase(id);
      is_enemy.erase(id);
      is_knife.erase(id);
      entities.erase(remove(entities.begin(), entities.end(), id),
                     entities.end());
      entities_marked_for_deletion_tmp.push_back(id);
    }
  }
  for (auto id : entities_marked_for_deletion_tmp) {
    is_marked_for_deletion.erase(id);
  }
  entities_marked_for_deletion_tmp.clear();
}