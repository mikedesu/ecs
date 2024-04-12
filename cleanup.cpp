#include "SDL_handler.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "generator_component.h"
#include "mPrint.h"
#include "sprite_component.h"
#include "transform_component.h"
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::exit;
using std::for_each;
using std::function;
using std::pair;
using std::remove;
using std::string;
using std::unordered_map;
using std::vector;

extern SDL_Renderer *renderer;
extern SDL_Texture *debug_texture;
extern SDL_Texture *debug_bg_texture;
extern SDL_Texture *target_texture;
extern SDL_Window *window;
extern SDL_Surface *text_surface;
extern TTF_Font *gFont;

extern unordered_map<string, SDL_Texture *> textures;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, generator_component> generators;
extern unordered_map<entity_id, bool> inputs;
extern unordered_map<entity_id, bool> is_rotating;
extern unordered_map<entity_id, bool> is_collidable;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_soulshard;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_flipped;
extern unordered_map<entity_id, bool> is_generator;
extern unordered_map<entity_id, bool> is_marked_for_deletion;
extern unordered_map<enemy_type, int> enemies_killed;
extern unordered_map<int, bool> is_pressed;

extern vector<entity_id> entities;
extern vector<entity_id> entities_marked_for_deletion_tmp;

function<void(pair<entity_id, bool>)> cleanup_entity_marked_for_deletion =
    [](auto kv) {
      entity_id id = kv.first;
      if (kv.second) {
        sprites.erase(id);
        transforms.erase(id);
        inputs.erase(id);
        is_rotating.erase(id);
        is_collidable.erase(id);
        is_enemy.erase(id);
        is_knife.erase(id);
        is_soulshard.erase(id);
        entities.erase(remove(entities.begin(), entities.end(), id),
                       entities.end());
        entities_marked_for_deletion_tmp.push_back(id);
      }
    };

void cleanup_textures();
void cleanup_and_exit_with_failure();
void cleanup_and_exit_with_failure_mprint(string message);
void cleanup_entities_marked_for_deletion();

void cleanup_textures() {
  for_each(textures.begin(), textures.end(),
           [](auto kv) { SDL_DestroyTexture(kv.second); });
  textures.clear();
}

void cleanup() {
  // mPrint("cleaning up");
  // mPrint("cleaning up entities");
  entities.clear();
  // mPrint("cleaning up entities_marked_for_deletion");
  entities_marked_for_deletion_tmp.clear();
  // mPrint("cleaning up maps");
  is_pressed.clear();
  is_rotating.clear();
  is_collidable.clear();
  is_enemy.clear();
  is_knife.clear();
  is_marked_for_deletion.clear();
  is_generator.clear();
  is_flipped.clear();
  is_soulshard.clear();
  enemies_killed.clear();
  sprites.clear();
  transforms.clear();
  generators.clear();
  inputs.clear();
  // mPrint("cleaning up SDL");
  // mPrint("cleaning up SDL textures");
  cleanup_textures();
  // mPrint("cleaning up debug texture");
  SDL_DestroyTexture(debug_texture);
  // mPrint("cleaning up debug bg texture");
  SDL_DestroyTexture(debug_bg_texture);
  // mPrint("cleaning up target texture");
  SDL_DestroyTexture(target_texture);
  // mPrint("cleaning up renderer");
  SDL_DestroyRenderer(renderer);
  if (text_surface != nullptr) {
    // mPrint("cleaning up text surface");
    SDL_FreeSurface(text_surface);
  }
  // mPrint("cleaning up window");
  SDL_DestroyWindow(window);
  // mPrint("cleaning up TTF");
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
  for_each(is_marked_for_deletion.begin(), is_marked_for_deletion.end(),
           cleanup_entity_marked_for_deletion);

  for_each(entities_marked_for_deletion_tmp.begin(),
           entities_marked_for_deletion_tmp.end(),
           [](entity_id id) { is_marked_for_deletion.erase(id); });

  entities_marked_for_deletion_tmp.clear();
}
