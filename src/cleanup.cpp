#include "SDL_handler.h"
#include "components.h"
#include "enemy_bullets.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "entity_type.h"
#include "mPrint.h"
#include "powerup_type.h"
#include <SDL_joystick.h>
#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::exit;
using std::for_each;
using std::function;
using std::map;
using std::pair;
using std::remove;
using std::string;
using std::unordered_map;
using std::vector;

extern SDL_Renderer *renderer;
extern SDL_Texture *debug_texture;
extern SDL_Texture *stopwatch_texture;
extern SDL_Texture *debug_bg_texture;
extern SDL_Texture *target_texture;
extern SDL_Window *window;
extern SDL_Surface *text_surface;
extern SDL_Surface *stopwatch_surface;
extern SDL_Joystick *joystick;
extern TTF_Font *gFont;
extern TTF_Font *stopwatch_font;
extern TTF_Font *gameover_font;

// extern int current_frame_count;

extern unordered_map<entity_id, int> knife_charges;
extern map<entity_id, sprite_component> sprites;
extern map<entity_id, sprite_component> bg_sprites;
extern unordered_map<entity_id, int> explosion_frames;
extern unordered_map<powerup_type, int> powerups_collected;
extern unordered_map<string, SDL_Texture *> textures;
extern unordered_map<entity_id, int> hitpoints;
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
// extern unordered_map<entity_id, bool> is_blood_pixel;
extern unordered_map<entity_id, int> blood_pixel_lifetime;
extern unordered_map<enemy_type, int> enemies_killed;
extern unordered_map<entity_id, entity_type> entity_types;
extern unordered_map<int, bool> is_pressed;
extern unordered_map<entity_id, powerup_type> powerup_types;
extern unordered_map<entity_id, bool> is_powerup;
extern unordered_map<string, int> num_clips;
extern unordered_map<enemy_type, enemy_bullet_definition>
    enemy_bullet_definitions;
extern vector<entity_id> entities;
extern vector<entity_id> entities_marked_for_deletion_tmp;
extern vector<double> bat_vx_vec;
extern vector<int> bat_y_vec;

void cleanup_textures();
void cleanup_and_exit_with_failure();
void cleanup_and_exit_with_failure_mprint(string message);
void cleanup_entities_marked_for_deletion();

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
        knife_charges.erase(id);
        is_soulshard.erase(id);
        is_flipped.erase(id);
        is_generator.erase(id);
        entity_types.erase(id);
        bg_sprites.erase(id);
        powerup_types.erase(id);
        is_powerup.erase(id);
        hitpoints.erase(id);
        // is_marked_for_deletion.erase(id);
        // is_blood_pixel.erase(id);
        blood_pixel_lifetime.erase(id);
        explosion_frames.erase(id);
        entities.erase(remove(entities.begin(), entities.end(), id),
                       entities.end());
        entities_marked_for_deletion_tmp.push_back(id);
      }
    };

void cleanup_textures() {
  for_each(textures.begin(), textures.end(),
           [](auto kv) { SDL_DestroyTexture(kv.second); });
  textures.clear();
  num_clips.clear();
}

void cleanup_data_structures() {
  explosion_frames.clear();
  entities.clear();
  entities_marked_for_deletion_tmp.clear();
  blood_pixel_lifetime.clear();
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
  // is_blood_pixel.clear();
  entity_types.clear();
  bg_sprites.clear();
  bat_y_vec.clear();
  bat_vx_vec.clear();
  powerup_types.clear();
  is_powerup.clear();
  hitpoints.clear();
  powerups_collected.clear();
  knife_charges.clear();
}

// void cleanup_enemy_bullet_definitions() {
//   enemy_bullet_definitions.clear();
// }

void cleanup() {
  cleanup_data_structures();
  cleanup_textures();
  enemy_bullet_definitions.clear();
  SDL_DestroyTexture(debug_texture);
  SDL_DestroyTexture(debug_bg_texture);
  SDL_DestroyTexture(stopwatch_texture);
  SDL_DestroyTexture(target_texture);
  SDL_DestroyRenderer(renderer);
  if (text_surface) {
    SDL_FreeSurface(text_surface);
  }
  if (stopwatch_surface) {
    SDL_FreeSurface(stopwatch_surface);
  }
  SDL_DestroyWindow(window);
  TTF_CloseFont(gFont);
  TTF_CloseFont(stopwatch_font);
  TTF_CloseFont(gameover_font);
  TTF_Quit();
  IMG_Quit();
  SDL_JoystickClose(joystick);
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
