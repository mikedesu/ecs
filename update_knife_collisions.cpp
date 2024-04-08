
#include "SDL_handler.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "sprite_component.h"
#include <random>
#include <unordered_map>

using std::default_random_engine;
using std::uniform_real_distribution;
using std::unordered_map;

extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_marked_for_deletion;
extern unordered_map<enemy_type, int> enemies_killed;
extern int num_collisions;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> coin_spawn_rate_distribution;

extern void spawn_coin(int x, int y);

#define COIN_SPAWN_RATE 0.25

void update_knife_collisions() {
  for (auto kv : is_knife) {
    entity_id id = kv.first;
    sprite_component knife = sprites[id];
    for (auto kv2 : is_enemy) {
      entity_id enemy_id = kv2.first;
      bool is_enemy_val = kv2.second;
      sprite_component enemy = sprites[enemy_id];
      if (!is_enemy_val) {
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
        double roll = coin_spawn_rate_distribution(rng_generator);
        if (roll < COIN_SPAWN_RATE) {
          spawn_coin(enemy.dest.x, enemy.dest.y);
        }
      }
    }
  }
}
