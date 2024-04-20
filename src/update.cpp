// #include "bg_entity_type.h"
#include "components.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "entity_type.h"
// #include "gameconfig.h"
#include "mPrint.h"
#include "powerup_type.h"
#include "rotation_pair.h"
#include "sprite_pair.h"
#include "transform_pair.h"
#include <algorithm>
#include <functional>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#define POWERUP_COST 5

using std::default_random_engine;
using std::for_each;
using std::function;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

const int cooldown_min = 10;

// extern gameconfig config;
extern unordered_map<string, size_t> config;
extern int current_knife_speed;
extern int frame_count;
extern int num_collisions;
extern int num_knives;
extern int max_num_knives;
extern int player_soulshards;
extern int total_soulshards_collected;
extern int player_health;
extern int player_max_health;
extern int num_enemies_escaped;
extern int soulshard_spawn_rate;
extern int current_knife_cooldown;
extern int knife_cooldown;
extern int knife_charge;
extern entity_id player_id;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> soulshard_spawn_rate_distribution;

extern unordered_map<entity_id, powerup_type> powerup_types;
extern unordered_map<entity_id, enemy_type> enemy_types;
extern unordered_map<entity_id, entity_type> entity_types;
extern unordered_map<entity_id, generator_component> generators;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, sprite_component> bg_sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, transform_component> bg_transforms;
extern unordered_map<entity_id, bool> is_soulshard;
extern unordered_map<entity_id, bool> is_rotating;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_generator;
extern unordered_map<entity_id, bool> is_powerup;
extern unordered_map<entity_id, bool> is_marked_for_deletion;
extern unordered_map<entity_id, bool> is_blood_pixel;
extern unordered_map<enemy_type, int> enemies_killed;
extern unordered_map<powerup_type, int> powerups_collected;
extern unordered_map<entity_id, double> rotation_speeds;
extern vector<entity_id> entities;

extern void spawn_soulshard(const int x, const int y);
extern void spawn_eyeball();
extern void spawn_powerup();
extern void spawn_bat();
extern void spawn_blood_pixels(const int x, const int y, const int n);
extern double distance(const int x1, const int y1, const int x2, const int y2);

function<void()> handle_update_skull_collision_knife = []() {
  num_knives++;
  knife_charge++;
  if (num_knives > max_num_knives) {
    num_knives = max_num_knives;
    knife_cooldown = 0;
  }
};

function<void()> handle_update_skull_collision_soulshard = []() {
  player_soulshards++;
  total_soulshards_collected++;
  if (player_soulshards >= POWERUP_COST) {
    spawn_powerup();
    player_soulshards -= POWERUP_COST;
  }
};

function<void(entity_id)> handle_magneticism = [](entity_id id) {
  const sprite_component s = sprites[player_id];
  const sprite_component o = sprites[id];
  const int pad_w = s.dest.w / 2;
  const int pad_h = s.dest.h / 2;
  const int x = s.dest.x + pad_w;
  const int y = s.dest.y + pad_h;
  const int x1 = o.dest.x + o.dest.w / 2;
  const int y1 = o.dest.y + o.dest.h / 2;
  const double dist = distance(x, y, x1, y1);
  const double threshold = 400.0;
  const int v = 8;
  if (dist < threshold) {
    // magnetically move the soulshard towards the player
    if (x1 < x) {
      transforms[id].vx = v;
    } else if (x1 > x + pad_w) {
      transforms[id].vx = -v;
    } else {
      transforms[id].vx = 0;
    }

    if (y1 < y) {
      transforms[id].vy = v;
    } else if (y1 > y + pad_h) {
      transforms[id].vy = -v;
    } else {
      transforms[id].vy = 0;
    }
  } else {
    transforms[id].vx = -1;
    transforms[id].vy = 0;
  }
};

function<void(entity_id)> handle_update_skull_collision_powerup =
    [](entity_id id) {
      powerup_type type = powerup_types[id];
      switch (type) {
      case POWERUP_TYPE_KNIFE_COOLDOWN:
        current_knife_cooldown -= 5;
        if (current_knife_cooldown < 10) {
          current_knife_cooldown = 10;
        }
        break;
      case POWERUP_TYPE_KNIFE_QUANTITY:
        num_knives++;
        max_num_knives++;
        if (num_knives > max_num_knives) {
          num_knives = max_num_knives;
        }
        break;
      case POWERUP_TYPE_KNIFE_SPEED:
        current_knife_speed += 1;
        break;
      case POWERUP_TYPE_HEART:
        player_health++;
        if (player_health > player_max_health) {
          player_health = player_max_health;
        }
        break;
      default:
        break;
      }

      powerups_collected[type]++;
    };

function<void(entity_id)> update_skull_collision = [](const entity_id id) {
  if (id == player_id) {
    return;
  }
  sprite_component skull = sprites[player_id];
  sprite_component other = sprites[id];
  if (SDL_HasIntersection(&skull.dest, &other.dest)) {
    num_collisions++;
    // might be able to optimize by using a map with the entity type
    switch (entity_types[id]) {
    case ENTITY_TYPE_KNIFE:
      is_marked_for_deletion[id] = true;
      handle_update_skull_collision_knife();
      break;
    case ENTITY_TYPE_SOULSHARD:
      is_marked_for_deletion[id] = true;
      handle_update_skull_collision_soulshard();
      break;
    case ENTITY_TYPE_ENEMY:
      is_marked_for_deletion[id] = true;
      player_health--;
      break;
    case ENTITY_TYPE_ITEM:
      is_marked_for_deletion[id] = true;
      handle_update_skull_collision_powerup(id);
      break;
    default:
      break;
    }
  }

  switch (entity_types[id]) {
  case ENTITY_TYPE_SOULSHARD:
    // case ENTITY_TYPE_ITEM:
    handle_magneticism(id);
    break;
  default:
    break;
  }
};

function<void(transform_pair)> handle_bg_transform =
    [](const transform_pair t) {
      entity_id id = t.first;
      transform_component transform = t.second;
      sprite_component sprite = bg_sprites[id];
      transform.x += transform.vx;
      transform.y += transform.vy;

      // handle looping background
      if (transform.x < -sprite.src.w) {
        transform.x = config["window_width"];
      }
      // else if (transform.x > config["window_width"]) {
      //     transform.x = -sprite.src.w;
      //   }

      sprite.dest.x = transform.x;
      sprite.dest.y = transform.y;
      sprite.dest.w = sprite.src.w * transform.scale;
      sprite.dest.h = sprite.src.h * transform.scale;
      bg_sprites[id] = sprite;
      bg_transforms[id] = transform;
    };

// bounds checking, player cannot move beyond texture
function<void(const entity_id, transform_component &)> handle_skull_transform =
    [](const entity_id id, transform_component &transform) {
      sprite_component sprite = sprites[id];
      if (transform.x < 0) {
        transform.x = 0;
      } else if (transform.x > config["target_texture_width"] - sprite.dest.w) {
        transform.x = config["target_texture_width"] - sprite.dest.w;
      }
      if (transform.y < 0) {
        transform.y = 0;
      } else if (transform.y >
                 config["target_texture_height"] - sprite.dest.h) {
        transform.y = config["target_texture_height"] - sprite.dest.h;
      }
    };

function<void(const entity_id)> handle_enemy_transform =
    [](const entity_id id) {
      const int w = sprites[id].src.w;
      const int left = 2 * -w;
      const int window_width = config["window_width"];
      const int right = window_width + 2 * w;
      const int x = transforms[id].x;
      const bool mark = x < left || x > right;
      is_marked_for_deletion[id] = mark;
      if (mark) {
        num_enemies_escaped++;
      }
    };

function<void(const entity_id)> handle_knife_soulshard_transform =
    [](const entity_id id) {
      bool is_marked =
          transforms[id].x < 2 * -sprites[id].src.w ||
          transforms[id].x > config["window_width"] + 2 * sprites[id].src.w;
      is_marked_for_deletion[id] = is_marked;
      if (is_knife[id] && is_marked) {
        num_knives++;
        if (num_knives > max_num_knives) {
          num_knives = max_num_knives;
        }
      }
    };

function<void(const entity_id)> handle_powerup_transform =
    [](const entity_id id) {
      is_marked_for_deletion[id] =
          transforms[id].x < 2 * -sprites[id].src.w ||
          transforms[id].x > config["window_width"] + 2 * sprites[id].src.w ||
          transforms[id].y < 2 * -sprites[id].src.h ||
          transforms[id].y > config["window_width"] + 2 * sprites[id].src.h;
    };

function<void(const entity_id)> handle_blood_pixel_transform =
    [](const entity_id id) {
      const int left = 2 * -sprites[id].src.w;
      const int width = config["window_width"];
      const int right = width + 2 * sprites[id].src.w;
      const int top = 2 * -sprites[id].src.h;
      const int bottom = width + 2 * sprites[id].src.h;
      const int x = transforms[id].x;
      const int y = transforms[id].y;
      const bool mark = x < left || x > right || y < top || y > bottom;
      is_marked_for_deletion[id] = mark;
    };

function<void(transform_pair)> handle_transform = [](const transform_pair t) {
  entity_id id = t.first;
  transform_component transform = t.second;
  transform.x += transform.vx;
  transform.y += transform.vy;
  if (id == player_id) {
    handle_skull_transform(id, transform);
  }
  transforms[id] = transform;
  sprites[id].dest.x = transform.x;
  sprites[id].dest.y = transform.y;
  sprites[id].dest.w = sprites[id].src.w * transform.scale;
  sprites[id].dest.h = sprites[id].src.h * transform.scale;
  if (id != player_id && is_enemy[id]) {
    handle_enemy_transform(id);
  } else if (id != player_id && (is_knife[id] || is_soulshard[id])) {
    handle_knife_soulshard_transform(id);
  } else if (id != player_id && is_powerup[id]) {
    handle_powerup_transform(id);
  } else if (id != player_id && is_blood_pixel[id]) {
    handle_blood_pixel_transform(id);
  }
};

function<void(rotation_pair)> handle_rotation = [](const rotation_pair p) {
  entity_id id = p.first;
  if (p.second) {
    transform_component transform = transforms[p.first];
    if (is_knife[p.first]) {
      transform.angle += rotation_speeds[p.first];
    } else {
      transform.angle += 1.0;
    }
    transforms[id] = transform;
  }
};

function<void(entity_id, entity_id)> check_for_knife_collision_with_enemy =
    [](const entity_id id, const entity_id enemy_id) {
      enemy_type type = enemy_types[enemy_id];
      sprite_component enemy = sprites[enemy_id];
      sprite_component knife = sprites[id];
      if (SDL_HasIntersection(&knife.dest, &enemy.dest)) {
        const int x = enemy.dest.x + enemy.dest.w / 2;
        const int y = enemy.dest.y + enemy.dest.h / 2;
        int num_pixels = config["blood_pixel_count"];
        is_marked_for_deletion[enemy_id] = true;
        is_marked_for_deletion[id] = true;
        num_collisions++;
        enemies_killed[type]++;
        spawn_soulshard(enemy.dest.x, enemy.dest.y);
        spawn_blood_pixels(x, y, num_pixels);
        if (is_marked_for_deletion[id]) {
          num_knives++;
          if (num_knives > max_num_knives) {
            num_knives = max_num_knives;
          }
        }
      }
    };

function<void(entity_id)> check_for_knife_collision = [](const entity_id id) {
  if (!is_knife[id]) {
    return;
  }
  for (auto p : is_enemy) {
    entity_id enemy_id = p.first;
    if (id == enemy_id || is_blood_pixel[enemy_id] || is_soulshard[enemy_id] ||
        is_powerup[enemy_id]) {
      continue;
    }
    check_for_knife_collision_with_enemy(id, enemy_id);
  }
};

function<void(sprite_pair)> update_animation = [](const sprite_pair p) {
  entity_id id = p.first;
  if (sprites[id].is_animating && frame_count % 10 == 0) {
    sprites[id].current_clip =
        (sprites[id].current_clip + 1) % sprites[id].num_clips;
    sprites[id].src.x = sprites[id].current_clip * sprites[id].src.w;
  }
};

function<void(sprite_pair)> update_bg_animation = [](const sprite_pair p) {
  entity_id id = p.first;
  if (bg_sprites[id].is_animating && frame_count % 10 == 0) {
    bg_sprites[id].current_clip =
        (bg_sprites[id].current_clip + 1) % bg_sprites[id].num_clips;
    bg_sprites[id].src.x = bg_sprites[id].current_clip * bg_sprites[id].src.w;
  }
};

void update_bg_animations() {
  for_each(bg_sprites.begin(), bg_sprites.end(), update_bg_animation);
}

void update_bg_transform_components() {
  for_each(bg_transforms.begin(), bg_transforms.end(), handle_bg_transform);
}

void update_generators() {
  for (auto kv : is_generator) {
    entity_id id = kv.first;
    const bool active = generators[id].active;
    const int cooldown = generators[id].cooldown;
    const int cooldown_reduction = generators[id].cooldown_reduction;
    const int group = generators[id].group;
    if (active && frame_count % cooldown == 0) {
      switch (generators[id].type) {
      case ENEMY_TYPE_EYEBALL:
        spawn_eyeball();
        break;
      case ENEMY_TYPE_BAT:
        for (int i = 0; i < group; i++) {
          spawn_bat();
        }
        break;
      default:
        break;
      }
    }
    // if the generator has a "cooldown reduction" set to non-zero,
    // then every N frames, reduce the cooldown by half until we hit a minimum
    if (cooldown_reduction && frame_count > 0 &&
        frame_count % cooldown_reduction == 0 && cooldown > cooldown_min) {
      generators[id].cooldown = cooldown / 2;
    }
  }
}

void update_skull_collisions() {
  for_each(entities.begin(), entities.end(), update_skull_collision);
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

void update_knife_cooldown() {
  knife_cooldown--;
  if (knife_cooldown <= 0) {
    knife_cooldown = 0;
  }
}

void update() {
  update_bg_transform_components();
  update_bg_animations();
  update_transform_components();
  update_rotations();
  update_animations();
  update_collisions();
  update_generators();
  update_knife_cooldown();
}
