
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
extern int player_money;
extern int player_health;
extern entity_id player_id;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> coin_spawn_rate_distribution;

extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_coin;
extern unordered_map<entity_id, bool> is_marked_for_deletion;
extern unordered_map<enemy_type, int> enemies_killed;
extern vector<entity_id> entities;

// functions
// extern void spawn_coin(int x, int y);

void update_skull_collisions() {
  sprite_component skull = sprites[player_id];
  for (auto id : entities) {
    if (id == player_id) {
      continue;
    } else if (is_knife[id]) {
      continue;
    } else if (is_coin[id]) {
      sprite_component coin = sprites[id];
      if (SDL_HasIntersection(&skull.dest, &coin.dest)) {
        is_marked_for_deletion[id] = true;
        num_collisions++;
        player_money++;
        break;
      }
    } else if (is_enemy[id]) {
      sprite_component enemy = sprites[id];
      if (SDL_HasIntersection(&skull.dest, &enemy.dest)) {
        is_marked_for_deletion[id] = true;
        num_collisions++;
        player_health--;
        break;
      }
    }
  }
}
