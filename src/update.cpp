// #include "bg_entity_type.h"
#include "components.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "mPrint.h"
#include "powerup_type.h"
#include "rotation_pair.h"
#include "sprite_pair.h"
#include "transform_pair.h"
#include <algorithm>
#include <functional>
#include <random>
#include <unordered_map>
#include <vector>

#define POWERUP_COST 5

using std::default_random_engine;
using std::for_each;
using std::function;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

const int cooldown_min = 10;

extern int frame_count;
extern int num_collisions;
extern int num_knives;
extern int max_num_knives;
extern int player_soulshards;
extern int total_soulshards_collected;
extern int player_health;
extern int target_texture_width;
extern int target_texture_height;
extern int num_enemies_escaped;
extern int window_width;
extern int soulshard_spawn_rate;
extern entity_id player_id;
extern int current_knife_cooldown;
extern int knife_cooldown;
extern int knife_charge;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> soulshard_spawn_rate_distribution;

extern unordered_map<entity_id, powerup_type> powerup_types;
extern unordered_map<entity_id, enemy_type> enemy_types;
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
extern unordered_map<enemy_type, int> enemies_killed;
extern unordered_map<powerup_type, int> powerups_collected;
extern unordered_map<entity_id, double> rotation_speeds;
extern vector<entity_id> entities;

extern void spawn_soulshard(int x, int y);
extern void spawn_eyeball();
extern void spawn_powerup();
extern void spawn_bat();
extern void spawn_blood_pixel(int x, int y);
extern void spawn_blood_pixels(int x, int y, int n);
extern double distance(int x1, int y1, int x2, int y2);

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

function<void(entity_id)> handle_soulshard_magneticism = [](entity_id id) {
  sprite_component skull = sprites[player_id];
  sprite_component other = sprites[id];
  int cx = skull.dest.x + skull.dest.w / 2;
  int cy = skull.dest.y + skull.dest.h / 2;
  int cx1 = other.dest.x + other.dest.w / 2;
  int cy1 = other.dest.y + other.dest.h / 2;
  double dist = distance(cx, cy, cx1, cy1);
  const double threshold = 200.0;
  const int speed = 4;
  if (dist < threshold) {
    const int pad = 4;
    // magnetically move the soulshard towards the player
    transforms[id].vx = cx1 < cx - pad ? speed : cx1 > cx + pad ? -speed : 0;
    transforms[id].vy = cy1 < cy - pad ? speed : cy1 > cy + pad ? -speed : 0;
  } else {
    transforms[id].vx = -1;
    transforms[id].vy = 0;
  }
};

function<void(entity_id)> handle_update_skull_collision_powerup =
    [](entity_id id) {
      powerup_type type = powerup_types[id];
      if (type == POWERUP_TYPE_KNIFE_COOLDOWN) {
        current_knife_cooldown -= 5;
        if (current_knife_cooldown < 10) {
          current_knife_cooldown = 10;
        }
      } else if (type == POWERUP_TYPE_KNIFE_QUANTITY) {
        num_knives++;
        max_num_knives++;
        if (num_knives > max_num_knives) {
          num_knives = max_num_knives;
        }
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
    is_marked_for_deletion[id] = true;
    num_collisions++;
    if (is_knife[id]) {
      handle_update_skull_collision_knife();
    } else if (is_soulshard[id]) {
      handle_update_skull_collision_soulshard();
    } else if (is_enemy[id]) {
      player_health--;
    } else if (is_powerup[id]) {
      handle_update_skull_collision_powerup(id);
    }
  } else if (is_soulshard[id]) {
    handle_soulshard_magneticism(id);
  }
};

function<void(transform_pair)> handle_bg_transform =
    [](const transform_pair t) {
      entity_id id = t.first;
      transform_component transform = t.second;
      sprite_component sprite = bg_sprites[id];
      transform.x += transform.vx;
      transform.y += transform.vy;
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
      } else if (transform.x > target_texture_width - sprite.dest.w) {
        transform.x = target_texture_width - sprite.dest.w;
      }
      if (transform.y < 0) {
        transform.y = 0;
      } else if (transform.y > target_texture_height - sprite.dest.h) {
        transform.y = target_texture_height - sprite.dest.h;
      }
    };

function<void(const entity_id)> handle_enemy_transform =
    [](const entity_id id) {
      is_marked_for_deletion[id] = transforms[id].x < -sprites[id].src.w;
      if (is_marked_for_deletion[id]) {
        num_enemies_escaped++;
      }
    };

function<void(const entity_id)> handle_knife_soulshard_transform =
    [](const entity_id id) {
      bool is_marked = transforms[id].x < 2 * -sprites[id].src.w ||
                       transforms[id].x > window_width + 2 * sprites[id].src.w;
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
          transforms[id].x > window_width + 2 * sprites[id].src.w ||
          transforms[id].y < 2 * -sprites[id].src.h ||
          transforms[id].y > window_width + 2 * sprites[id].src.h;
    };

function<void(const entity_id)> handle_blood_pixel_transform =
    [](const entity_id id) {
      is_marked_for_deletion[id] =
          transforms[id].x < 2 * -sprites[id].src.w ||
          transforms[id].x > window_width + 2 * sprites[id].src.w ||
          transforms[id].y < 2 * -sprites[id].src.h ||
          transforms[id].y > window_width + 2 * sprites[id].src.h;
    };

extern unordered_map<entity_id, bool> is_blood_pixel;
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
  bool is_rotating = p.second;
  if (is_rotating) {
    transform_component transform = transforms[id];
    if (is_knife[id]) {
      transform.angle += rotation_speeds[id];
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
      spawn_soulshard(enemy.dest.x, enemy.dest.y);

      const int num_pixels = 20;
      const int x = enemy.dest.x + enemy.dest.w / 2;
      const int y = enemy.dest.y + enemy.dest.h / 2;
      spawn_blood_pixels(x, y, num_pixels);

      if (is_marked_for_deletion[id]) {
        num_knives++;
        if (num_knives > max_num_knives) {
          num_knives = max_num_knives;
        }
      }
    }
  }
};

function<void(sprite_pair)> update_animation = [](const sprite_pair p) {
  entity_id id = p.first;
  sprite_component sprite = sprites[id];
  if (sprite.is_animating && is_soulshard[id]) {
    if (frame_count % 10 == 0) {
      // mPrint("soulshard animation");
      sprite.current_clip = (sprite.current_clip + 1) % sprite.num_clips;
      sprite.src.x = sprite.current_clip * sprite.src.w;
      sprites[id] = sprite;
    }
  } else if (sprite.is_animating && enemy_types[id] == ENEMY_TYPE_BAT) {
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

void update_bg_animations() {
  for_each(bg_sprites.begin(), bg_sprites.end(), update_animation);
}

void update_bg_transform_components() {
  for_each(bg_transforms.begin(), bg_transforms.end(), handle_bg_transform);
}

void update_generators() {
  // for (auto kv : generators) {
  for (auto kv : is_generator) {
    entity_id id = kv.first;
    // generator_component generator = kv.second;
    if (generators[id].active && frame_count % generators[id].cooldown == 0) {
      switch (generators[id].type) {
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

    // if the generator has a "cooldown reduction" set to non-zero,
    // then every N frames, reduce the cooldown by half until we hit a minimum
    if (generators[id].cooldown_reduction && frame_count > 0 &&
        frame_count % generators[id].cooldown_reduction == 0 &&
        generators[id].cooldown > cooldown_min) {
      generators[id].cooldown = generators[id].cooldown / 2;
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