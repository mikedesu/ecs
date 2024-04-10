#include "enemy_type.h"
#include "entity_id.h"
#include "generator_component.h"
#include "powerup_type.h"
#include "rotation_pair.h"
#include "sprite_component.h"
#include "sprite_pair.h"
#include "transform_component.h"
#include "transform_pair.h"
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
extern int player_money;
extern int player_health;
extern int target_texture_width;
extern int target_texture_height;
extern int num_enemies_escaped;
extern int window_width;
extern entity_id player_id;
extern int current_knife_cooldown;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> coin_spawn_rate_distribution;

extern unordered_map<entity_id, powerup_type> powerup_types;
extern unordered_map<entity_id, enemy_type> enemy_types;
extern unordered_map<entity_id, generator_component> generators;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, bool> is_coin;
extern unordered_map<entity_id, bool> is_rotating;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_powerup;
extern unordered_map<entity_id, bool> is_marked_for_deletion;
extern unordered_map<enemy_type, int> enemies_killed;
extern unordered_map<powerup_type, int> powerups_collected;
extern vector<entity_id> entities;

extern void update_knife_collisions();
extern void update_skull_collisions();
extern void spawn_coin(int x, int y);
extern void spawn_eyeball();
extern void spawn_powerup();
extern void spawn_bat();

function<void(transform_pair)> handle_transform = [](const transform_pair t) {
  entity_id id = t.first;
  transform_component transform = t.second;
  sprite_component sprite = sprites[id];
  transform.x += transform.vx;
  transform.y += transform.vy;
  if (id == player_id) {
    // bounds checking, player cannot move beyond texture
    if (transform.x < 0) {
      transform.x = 0;
    } else if (transform.x > target_texture_width - sprite.dest.w) {
      transform.x = target_texture_width - sprite.dest.w;
    }
    if (transform.y < 0) {
      transform.y = 0;
    } else if (transform.y > target_texture_height - sprite.dest.h) {
      transform.y = target_texture_height - sprite.dest.h;
    }
  }
  sprite.dest.x = transform.x;
  sprite.dest.y = transform.y;
  sprite.dest.w = sprite.src.w * transform.scale;
  sprite.dest.h = sprite.src.h * transform.scale;
  sprites[id] = sprite;
  if (id != player_id && is_enemy[id]) {
    is_marked_for_deletion[id] = transform.x < -sprite.src.w;
    if (is_marked_for_deletion[id]) {
      num_enemies_escaped++;
    }
  } else if (id != player_id && (is_knife[id] || is_coin[id])) {
    is_marked_for_deletion[id] =
        transform.x < -sprite.src.w || transform.x > window_width;
  }
  transforms[id] = transform;
};

function<void(rotation_pair)> handle_rotation = [](const rotation_pair p) {
  entity_id id = p.first;
  bool is_rotating = p.second;
  if (is_rotating) {
    transform_component transform = transforms[id];
    if (is_knife[id]) {
      transform.angle += 4.0;
    } else {
      transform.angle += 1.0;
    }
    transforms[id] = transform;
  }
};

function<void(entity_id)> check_for_knife_collision = [](const entity_id id) {
  if (!is_knife[id]) {
    return;
  }
  sprite_component knife = sprites[id];
  for (auto enemy_id : entities) {
    if (id == enemy_id || !is_enemy[enemy_id]) {
      continue;
    }
    sprite_component enemy = sprites[enemy_id];
    if (SDL_HasIntersection(&knife.dest, &enemy.dest)) {
      is_marked_for_deletion[enemy_id] = true;
      is_marked_for_deletion[id] = true;
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
  } else if (enemy_types[id] == ENEMY_TYPE_BAT) {
    if (frame_count % 10 == 0) {
      sprite.current_clip = (sprite.current_clip + 1) % sprite.num_clips;
      sprite.src.x = sprite.current_clip * sprite.src.w;
      sprites[id] = sprite;
    }
  }

  else if (sprite.is_animating) {
    sprite.current_clip = (sprite.current_clip + 1) % sprite.num_clips;
    sprite.src.x = sprite.current_clip * sprite.src.w;
    sprites[id] = sprite;
  }
};

void update_generators() {
  for (auto kv : generators) {
    generator_component generator = kv.second;
    if (generator.active && frame_count % generator.cooldown == 0) {
      switch (generator.type) {
      case ENEMY_TYPE_EYEBALL:
        spawn_eyeball();
        break;
      case ENEMY_TYPE_BAT:
        spawn_bat();
        break;
      default:
        break;
      }
    }
  }
}

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

        if (player_money >= 5) {
          spawn_powerup();
          player_money -= 5;
        }

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
    } else if (is_powerup[id]) {
      sprite_component powerup = sprites[id];
      powerup_type type = powerup_types[id];
      if (SDL_HasIntersection(&skull.dest, &powerup.dest)) {
        is_marked_for_deletion[id] = true;
        num_collisions++;

        if (type == POWERUP_TYPE_KNIFE_COOLDOWN) {
          current_knife_cooldown -= 5;
          if (current_knife_cooldown < 5) {
            current_knife_cooldown = 5;
          }
        }
        powerups_collected[type]++;

        // player_health++;
        break;
      }
    }
  }
}

void update_animations() {
  for_each(sprites.begin(), sprites.end(), update_animation);
}

void update_knife_collisions() {
  for_each(entities.begin(), entities.end(), check_for_knife_collision);
}

void update_collisions() {
  update_knife_collisions();
  update_skull_collisions();
}

void update_rotations() {
  for_each(is_rotating.begin(), is_rotating.end(), handle_rotation);
}

void update_transform_components() {
  for_each(transforms.begin(), transforms.end(), handle_transform);
}

void update() {
  update_transform_components();
  update_rotations();
  update_animations();
  update_collisions();
  update_generators();
}
