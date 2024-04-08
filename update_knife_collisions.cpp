#include "SDL_handler.h"
#include "enemy_type.h"
#include "entity_id.h"
// #include "mPrint.h"
#include "sprite_component.h"
#include <algorithm>
#include <functional>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#define COIN_SPAWN_RATE 1

using std::default_random_engine;
using std::for_each;
using std::function;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

extern int num_collisions;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> coin_spawn_rate_distribution;

extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_marked_for_deletion;
extern unordered_map<enemy_type, int> enemies_killed;
extern vector<entity_id> entities;

// functions
extern void spawn_coin(int x, int y);

function<void(entity_id)> check_for_knife_collision = [](const entity_id id) {
  if (!is_knife[id]) {
    return;
  }
  sprite_component knife = sprites[id];
  for (auto enemy_id : entities) {
    if (id == enemy_id) {
      continue;
    }
    if (!is_enemy[enemy_id]) {
      continue;
    }
    sprite_component enemy = sprites[enemy_id];
    if (SDL_HasIntersection(&knife.dest, &enemy.dest)) {
      is_marked_for_deletion[enemy_id] = true;
      is_marked_for_deletion[id] = true;
      // mPrint("knife collision - id: " + to_string(id) +
      //        " enemy_id: " + to_string(enemy_id));
      num_collisions++;
      enemies_killed[ENEMY_TYPE_EYEBALL]++;
      double roll = coin_spawn_rate_distribution(rng_generator);
      if (roll < COIN_SPAWN_RATE) {
        spawn_coin(enemy.dest.x, enemy.dest.y);
      }
      break;
    }
  }
};

void update_knife_collisions() {
  for_each(entities.begin(), entities.end(), check_for_knife_collision);
}
