#include "enemy_type.h"
#include "entity_id.h"
#include "generator_component.h"
#include "sprite_component.h"
#include "sprite_pair.h"
#include <algorithm>
#include <functional>
#include <random>
#include <unordered_map>
#include <vector>

#define COIN_SPAWN_RATE 1

using std::default_random_engine;
using std::for_each;
using std::function;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

extern int frame_count;
extern int num_collisions;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> coin_spawn_rate_distribution;

extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, bool> is_coin;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_marked_for_deletion;
extern unordered_map<enemy_type, int> enemies_killed;
extern vector<entity_id> entities;

extern void update_knife_collisions();
extern void update_skull_collisions();
extern void spawn_coin(int x, int y);
extern void spawn_eyeball();

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

function<void(sprite_pair)> update_animation = [](const sprite_pair p) {
  entity_id id = p.first;
  sprite_component sprite = sprites[id];

  if (is_coin[id]) {
    if (frame_count % 10 == 0) {
      sprite.current_clip = (sprite.current_clip + 1) % sprite.num_clips;
      sprite.src.x = sprite.current_clip * sprite.src.w;
      sprites[id] = sprite;
    }
  } else if (sprite.is_animating) {
    sprite.current_clip = (sprite.current_clip + 1) % sprite.num_clips;
    sprite.src.x = sprite.current_clip * sprite.src.w;
    sprites[id] = sprite;
  }
};

void update_animations() {
  for_each(sprites.begin(), sprites.end(), update_animation);
}

void update_collisions() {
  update_knife_collisions();
  update_skull_collisions();
}

void update_knife_collisions() {
  for_each(entities.begin(), entities.end(), check_for_knife_collision);
}
extern unordered_map<entity_id, generator_component> generators;

void update_generators() {
  for (auto kv : generators) {
    // entity_id id = kv.first;
    generator_component generator = kv.second;
    if (generator.active && frame_count % generator.cooldown == 0) {
      switch (generator.type) {
      case ENEMY_TYPE_EYEBALL:
        spawn_eyeball();
        break;
      default:
        break;
      }
    }
  }
}
