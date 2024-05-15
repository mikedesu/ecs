#include "components.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "entity_type.h"
#include "mPrint.h"
#include "powerup_type.h"
#include "rotation_pair.h"
#include "sprite_pair.h"
#include "transform_pair.h"
#include <algorithm>
#include <functional>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#define POWERUP_COST 5

using std::default_random_engine;
using std::for_each;
using std::function;
using std::map;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

extern bool is_gameover;
extern int w;
extern int h;
extern int cooldown_min;
extern int powerups_onscreen;
extern int current_soulshard_magnetism_threshold;
extern int current_knife_speed;
extern int current_player_speed;
extern int frame_count;
extern int window_width;
extern int window_height;
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
extern map<entity_id, sprite_component> sprites;
extern map<entity_id, sprite_component> bg_sprites;
extern unordered_map<string, size_t> config;
extern unordered_map<entity_id, int> knife_charges;
extern unordered_map<entity_id, int> explosion_frames;
extern unordered_map<entity_id, int> hitpoints;
extern unordered_map<string, int> num_clips;
extern unordered_map<string, SDL_Texture *> textures;
extern unordered_map<entity_id, powerup_type> powerup_types;
extern unordered_map<entity_id, enemy_type> enemy_types;
extern unordered_map<entity_id, entity_type> entity_types;
extern unordered_map<entity_id, generator_component> generators;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, transform_component> bg_transforms;
extern unordered_map<entity_id, bool> is_soulshard;
extern unordered_map<entity_id, bool> is_rotating;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_generator;
extern unordered_map<entity_id, bool> is_powerup;
extern unordered_map<entity_id, bool> is_marked_for_deletion;
extern unordered_map<entity_id, int> blood_pixel_lifetime;
extern unordered_map<entity_id, double> rotation_speeds;
extern unordered_map<enemy_type, int> enemies_killed;
extern unordered_map<powerup_type, int> powerups_collected;
extern vector<entity_id> entities;

extern void spawn_soulshard(const int x, const int y);
extern void spawn_powerup();
// extern void spawn_bat(const double x, const double y, const double vx,
//                       const double vy, const double scale);
extern void spawn_bats(const double x, const double y, const double scale,
                       const double vx, const double vy, const int number,
                       const int hp);
extern void spawn_eyeball(const double x, const double y, const double vx,
                          const double vy, const double scale, const int hp);
extern void spawn_goblin(const double x, const double y, const double vx,
                         const double vy, const double scale, const int hp);
void spawn_goblin_bullet(entity_id id);

extern void spawn_small_explosion(const int x, const int y);
extern void spawn_blood_pixels(const int x, const int y, const int n);
extern void spawn_small_explosion(const int x, const int y);

inline double distance(const int x1, const int y1, const int x2, const int y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

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
  if (player_soulshards >= POWERUP_COST && powerups_onscreen < 11) {
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
  const int v = 8;
  if (dist < current_soulshard_magnetism_threshold) {
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
      case POWERUP_TYPE_SKULL_SPEED:
        if (current_player_speed < 20) {
          current_player_speed += 2;
        }
        break;
      case POWERUP_TYPE_MAGNETISM_THRESHOLD:
        current_soulshard_magnetism_threshold += 20;
        break;
      case POWERUP_TYPE_KNIFE_COOLDOWN:
        if (current_knife_cooldown > 10) {
          current_knife_cooldown -= 5;
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
        if (current_knife_speed < 10) {
          current_knife_speed += 1;
        }

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
      powerups_onscreen--;
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
      sprites[player_id].dmg_frames = 10;
      player_health--;
      if (player_health <= 0) {
        mPrint("Gameover!");
        is_gameover = true;
      }
      break;
    case ENTITY_TYPE_ITEM:
      is_marked_for_deletion[id] = true;
      handle_update_skull_collision_powerup(id);
      break;
    case ENTITY_TYPE_ENEMY_BULLET:
      is_marked_for_deletion[id] = true;
      sprites[player_id].dmg_frames = 10;
      player_health--;
      if (player_health <= 0) {
        mPrint("Gameover!");
        is_gameover = true;
      }
      break;

    default:
      break;
    }
  }
  switch (entity_types[id]) {
  case ENTITY_TYPE_SOULSHARD:
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
      transform_component &bg_transform = bg_transforms[id];
      sprite_component &sprite = bg_sprites[id];
      bg_transform.x += transform.vx;
      bg_transform.y += transform.vy;
      // handle looping background
      if (transform.x < -bg_sprites[id].src.w * transform.scale) {
        bg_transform.x = config["window_width"];
      }
      sprite.dest.x = transform.x;
      sprite.dest.y = transform.y;
      sprite.dest.w = sprite.src.w * transform.scale;
      sprite.dest.h = sprite.src.h * transform.scale;
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
      if (entity_types[id] == ENTITY_TYPE_KNIFE && is_marked) {
        num_knives++;
        if (num_knives > max_num_knives) {
          num_knives = max_num_knives;
        }
      }
    };

function<void(transform_pair)> handle_transform = [](const transform_pair t) {
  entity_id id = t.first;
  transform_component transform = t.second;
  transform.x += transform.vx;
  transform.y += transform.vy;
  const double scale = transform.scale;
  double x = transform.x;
  double y = transform.y;
  double src_w = sprites[id].src.w;
  double src_h = sprites[id].src.h;
  double dest_w = src_w * scale;
  double dest_h = src_h * scale;
  if (id == player_id) {
    if (x < 0) {
      transform.x = 0;
    } else if (x > config["target_texture_width"] - dest_w) {
      transform.x = config["target_texture_width"] - dest_w;
    }
    if (y < 0) {
      transform.y = 0;
    } else if (y > config["target_texture_height"] - dest_h) {
      transform.y = config["target_texture_height"] - dest_h;
    }
  }
  transforms[id].x = transform.x;
  transforms[id].y = transform.y;
  sprites[id].dest.x = transform.x;
  sprites[id].dest.y = transform.y;
  sprites[id].dest.w = dest_w;
  sprites[id].dest.h = dest_h;
  switch (entity_types[id]) {
  case ENTITY_TYPE_ENEMY: {
    const int w = sprites[id].src.w;
    const int left = 2 * -w * scale;
    const int right = window_width + (2 * w * scale);
    const int x = transforms[id].x;
    const bool mark = x < left || x > right;
    is_marked_for_deletion[id] = mark;
    if (mark) {
      num_enemies_escaped++;
    }
  } break;
  case ENTITY_TYPE_KNIFE: {
    bool is_marked = transforms[id].x < 2 * -sprites[id].src.w ||
                     transforms[id].x > window_width + 2 * sprites[id].src.w;
    is_marked_for_deletion[id] = is_marked;
    if (is_marked) {
      num_knives++;
      if (num_knives > max_num_knives) {
        num_knives = max_num_knives;
      }
    }
  } break;
  case ENTITY_TYPE_SOULSHARD: {
    bool is_marked = transforms[id].x < 2 * -sprites[id].src.w ||
                     transforms[id].x > window_width + 2 * sprites[id].src.w;
    is_marked_for_deletion[id] = is_marked;
  } break;
  case ENTITY_TYPE_ITEM: {
    const bool mark = transforms[id].x < 2 * -sprites[id].src.w ||
                      transforms[id].x > window_width + 2 * sprites[id].src.w ||
                      transforms[id].y < 2 * -sprites[id].src.h ||
                      transforms[id].y > window_width + 2 * sprites[id].src.h;
    is_marked_for_deletion[id] = mark;
    if (mark) {
      powerups_onscreen--;
    }
  } break;
  case ENTITY_TYPE_PARTICLE: {
    blood_pixel_lifetime[id]--;
    is_marked_for_deletion[id] = blood_pixel_lifetime[id] <= 0;
  } break;

  case ENTITY_TYPE_ENEMY_BULLET: {
    bool is_marked = transforms[id].x < 2 * -sprites[id].src.w ||
                     transforms[id].x > window_width + 2 * sprites[id].src.w ||
                     transforms[id].y < 2 * -sprites[id].src.h ||
                     transforms[id].y > window_width + 2 * sprites[id].src.h;
    is_marked_for_deletion[id] = is_marked;

  } break;

  default:
    break;
  }
};

function<void(rotation_pair)> handle_rotation = [](const rotation_pair p) {
  entity_id id = p.first;
  if (p.second) {
    transform_component transform = transforms[id];
    switch (entity_types[id]) {
    case ENTITY_TYPE_KNIFE:
      transform.angle += rotation_speeds[id];
      break;
    case ENTITY_TYPE_ENEMY_BULLET:
      transform.angle += 10.0;
      break;
    default:
      transform.angle += 1.0;
      break;
    }
    transforms[id] = transform;
  }
};

// REFACTOR
function<void(entity_id, entity_id)> check_for_knife_collision_with_enemy =
    [](const entity_id id, const entity_id enemy_id) {
      enemy_type type = enemy_types[enemy_id];
      sprite_component enemy = sprites[enemy_id];
      sprite_component knife = sprites[id];
      if (SDL_HasIntersection(&knife.dest, &enemy.dest)) {
        // knife collides with enemy
        // knife damage is a function of current knife charge
        // atm, we have no way to track every knive's charge
        // we need a map that stores entity_id,int
        // we might be able to get rid of is_knife
        const int knife_charge = knife_charges[id];
        const int damage = 1 + knife_charge;
        const int x = enemy.dest.x + enemy.dest.w / 2;
        const int y = enemy.dest.y + enemy.dest.h / 2;
        spawn_small_explosion(x, y);
        hitpoints[enemy_id] -= damage;
        sprites[enemy_id].dmg_frames = 8;
        // delete the knife
        is_marked_for_deletion[id] = true;
        num_knives++;
        if (num_knives > max_num_knives) {
          num_knives = max_num_knives;
        }
        // mPrint("enemy hp is now: " + to_string(hitpoints[enemy_id]));
        if (hitpoints[enemy_id] <= 0) {
          const int x = enemy.dest.x + enemy.dest.w / 2;
          const int y = enemy.dest.y + enemy.dest.h / 2;
          const int num_pixels = config["blood_pixel_count"];
          is_marked_for_deletion[enemy_id] = true;
          num_collisions++;
          enemies_killed[type]++;
          spawn_soulshard(x, y);
          spawn_blood_pixels(x, y, num_pixels);
        }
      }
    };

function<void(pair<entity_id, bool>)> check_for_knife_collision =
    [](const pair<entity_id, bool> knife) {
      entity_id id = knife.first;
      for (auto p : is_enemy) {
        entity_id enemy_id = p.first;
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

inline void update_bg_animations() {
  for_each(bg_sprites.begin(), bg_sprites.end(), update_bg_animation);
}

inline void update_bg_transform_components() {
  for_each(bg_transforms.begin(), bg_transforms.end(), handle_bg_transform);
}

// void handle_eyeball_generator(entity_id id, const double scale) {
void handle_eyeball_generator(entity_id id) {
  screen_position_t position = generators[id].screen_position;
  int x = -1;
  int y = -1;
  int vx_dir = 0;
  int vy_dir = 0;
  const int hp = 4;
  int spawn_count = generators[id].spawn_count;
  double scale = generators[id].scale;
  SDL_QueryTexture(textures["eyeball"], NULL, NULL, &w, &h);
  switch (position) {
  case SCREEN_POSITION_LEFT: {

    x = scale * -w / num_clips["eyeball"];

    // y = config["target_texture_height"] / 2;
    y = rand() % (int)(config["target_texture_height"] - h * scale);

    vx_dir = 1.0;
  } break;
  case SCREEN_POSITION_RIGHT: {

    x = config["target_texture_width"];

    // y = config["target_texture_height"] / 2;
    y = rand() % (int)(config["target_texture_height"] - h * scale);

    vx_dir = -1.0;
  } break;
  default:
    break;
  }
  if (spawn_count > 0 || spawn_count == -1) {
    spawn_eyeball(x, y, vx_dir, vy_dir, scale, hp);
    if (spawn_count > 0) {
      generators[id].spawn_count--;
    }
  }
}

void handle_goblin_generator(entity_id id) {
  // at the moment, groupnumber and screen position aren't used
  // screen_position_t position = generators[id].screen_position;
  int x = -1;
  int y = -1;
  int vx_dir = 0;
  int vy_dir = 0;
  int hp = generators[id].hp;
  // int group = generators[id].group;
  int spawn_count = generators[id].spawn_count;
  double scale = generators[id].scale;
  string key = "goblin";
  SDL_QueryTexture(textures[key], NULL, NULL, &w, &h);
  x = config["target_texture_width"];
  y = config["target_texture_height"] - h * scale;
  vx_dir = -2.0; // flipped
  if (spawn_count > 0 || spawn_count == -1) {
    spawn_goblin(x, y, vx_dir, vy_dir, scale, hp);
    // decrement the spawn count
    if (spawn_count > 0) {
      generators[id].spawn_count--;
    }
  }
}

// void handle_bat_generator(entity_id id, const double scale) {
void handle_bat_generator(entity_id id) {
  screen_position_t position = generators[id].screen_position;
  int x = -1;
  int y = -1;
  int vx_dir = 0;
  int vy_dir = 0;
  int hp = generators[id].hp;
  int group = generators[id].group;
  int spawn_count = generators[id].spawn_count;
  double scale = generators[id].scale;
  switch (position) {
  case SCREEN_POSITION_LEFT: {
    SDL_QueryTexture(textures["bat"], NULL, NULL, &w, &h);
    x = scale * -w / num_clips["bat"];
    y = config["target_texture_height"] / 2;
    vx_dir = -1.0; // flipped
  } break;
  case SCREEN_POSITION_RIGHT: {
    x = config["target_texture_width"];
    y = config["target_texture_height"] / 2;
    vx_dir = 1.0; // flipped
  } break;
  default:
    break;
  }

  if (spawn_count > 0 || spawn_count == -1) {
    spawn_bats(x, y, scale, vx_dir, vy_dir, group, hp);

    // decrement the spawn count
    if (spawn_count > 0) {
      generators[id].spawn_count--;
    }
  }
}

// REFACTORING
void update_generators() {
  for (auto kv : is_generator) {
    entity_id id = kv.first;
    const int cooldown = generators[id].cooldown;
    const int reduction = generators[id].cooldown_reduction;
    if (frame_count >= generators[id].frame_begin) {
      if (generators[id].active && frame_count % cooldown == 0) {
        // mPrint("frame_count: " + to_string(frame_count));
        switch (generators[id].type) {
        case ENEMY_TYPE_EYEBALL: {
          handle_eyeball_generator(id);
        } break;
        case ENEMY_TYPE_BAT: {
          handle_bat_generator(id);
        } break;
        case ENEMY_TYPE_GOBLIN: {
          handle_goblin_generator(id);
        } break;
        default:
          break;
        }
      }
      // if the generator has "cooldown reduction" set to nonzero,
      // then every N frames, reduce the cooldown by half until we hit a
      // minimum
      bool do_reduce = reduction && frame_count > 0;
      do_reduce = do_reduce && cooldown > cooldown_min;
      do_reduce = do_reduce && frame_count % reduction == 0;
      if (do_reduce) {
        generators[id].cooldown = cooldown / 2;
      }
    }
  }
}

inline void update_skull_collisions() {
  for_each(entities.begin(), entities.end(), update_skull_collision);
}

inline void update_animations() {
  for_each(sprites.begin(), sprites.end(), update_animation);
}

inline void update_knife_collisions() {
  for_each(is_knife.begin(), is_knife.end(), check_for_knife_collision);
}

inline void update_collisions() {
  update_knife_collisions();
  update_skull_collisions();
}

inline void update_rotations() {
  for_each(is_rotating.begin(), is_rotating.end(), handle_rotation);
}

inline void update_transform_components() {
  for_each(transforms.begin(), transforms.end(), handle_transform);
}

inline void update_knife_cooldown() {
  knife_cooldown = knife_cooldown > 0 ? knife_cooldown - 1 : 0;
}

inline void update_explosions() {
  for (auto kv : explosion_frames) {
    entity_id id = kv.first;
    int frames = kv.second;
    if (frames <= 0) {
      is_marked_for_deletion[id] = true;
    } else {
      explosion_frames[id]--;
    }
  }
}

inline void update_entity_special_actions() {
  for (auto kv : is_enemy) {
    entity_id id = kv.first;

    // eventually we will be able to define special actions via JSON file in a
    // similar manner to everything else so as to avoid hard-coding the cases
    // and frame frequencies
    switch (enemy_types[id]) {
    case ENEMY_TYPE_GOBLIN: {
      if (frame_count % 120 == 0) {
        // mPrint("spawn goblin knife");

        spawn_goblin_bullet(id);
      }
    } break;
    }
  }
}

void update() {
  update_bg_transform_components();
  update_bg_animations();
  update_transform_components();
  update_animations();
  update_rotations();
  update_collisions();
  update_generators();
  update_explosions();
  update_knife_cooldown();

  update_entity_special_actions();
}
