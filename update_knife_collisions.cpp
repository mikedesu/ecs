
#include "enemy_type.h"
#include "entity_id.h"
#include "sprite_component.h"
#include <unordered_map>

#include "SDL_handler.h"

using std::unordered_map;

extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_marked_for_deletion;
extern unordered_map<enemy_type, int> enemies_killed;
extern int num_collisions;

void update_knife_collisions() {
  for (auto kv : is_knife) {
    entity_id id = kv.first;
    sprite_component knife = sprites[id];
    // SDL_Rect &knife_rect = knife.dest;
    for (auto kv2 : is_enemy) {
      entity_id enemy_id = kv2.first;
      bool is_enemy = kv2.second;
      sprite_component enemy = sprites[enemy_id];
      // SDL_Rect &enemy_rect = enemy.dest;
      if (!is_enemy) {
        continue;
      }
      if (SDL_HasIntersection(&knife.dest, &enemy.dest)) {
        // at some point we will have to distinguish enemies by type
        // if (enemy_type_for_id[id] == ENEMY_TYPE_EYEBALL) {
        is_marked_for_deletion[enemy_id] = true;
        is_marked_for_deletion[id] = true;
        // mPrint("knife collision with enemy id " + std::to_string(enemy_id));
        num_collisions++;
        enemies_killed[ENEMY_TYPE_EYEBALL]++;
      }
    }
  }
}
