#include "SDL_handler.h"
#include "components.h"
#include "entity_id.h"
#include "entity_type.h"
// #include "mPrint.h"
#include "powerup_type.h"
#include <SDL_render.h>
#include <algorithm>
#include <cassert>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#define DEFAULT_POWERUP_VX -1.0

using std::default_random_engine;
using std::mt19937;
using std::shuffle;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

extern mt19937 g;

extern vector<int> bat_y_vec;
extern unordered_map<string, size_t> config;
extern entity_id player_id;
extern unordered_map<entity_id, bool> inputs;
extern unordered_map<string, SDL_Texture *> textures;
extern int w;
extern int h;
extern int knife_cooldown;
extern int current_knife_speed;
extern int current_player_speed;
extern int current_knife_cooldown;
extern int num_knives;
extern int knife_charge;
extern int max_num_knives;
extern int num_knives_fired;
extern int player_money;
extern int player_health;
extern int player_max_health;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> unit_distribution;
extern uniform_real_distribution<double> eyeball_vx_distribution;
extern uniform_real_distribution<double> texture_height_distribution;
extern uniform_real_distribution<double> blood_velocity_positive_distribution;
extern uniform_real_distribution<double> blood_velocity_negative_distribution;
extern uniform_real_distribution<double> blood_velocity_distribution;

extern unordered_map<entity_id, entity_type> entity_types;

extern vector<double> bat_vx_vec;
extern unordered_map<entity_id, bool> is_blood_pixel;
extern unordered_map<entity_id, int> blood_pixel_lifetime;

extern unordered_map<entity_id, powerup_type> powerup_types;
extern unordered_map<entity_id, enemy_type> enemy_types;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, generator_component> generators;
extern unordered_map<entity_id, bool> is_soulshard;
extern unordered_map<entity_id, bool> is_flipped;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_rotating;
extern unordered_map<entity_id, bool> is_collidable;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_powerup;
extern unordered_map<entity_id, bool> is_generator;
extern unordered_map<powerup_type, int> powerups_collected;
extern unordered_map<entity_id, double> rotation_speeds;
extern vector<entity_id> entities;

extern entity_id get_next_entity_id();

void spawn_knife();
void spawn_knife_no_cooldown_no_count_check();
void spawn_bat(const double x, const double y, const double vx, const double vy,
               const double scale);

entity_id spawn_entity(const string key, const bool is_animating,
                       const int num_clips, const int x, const int y) {
  SDL_Texture *t = textures[key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  w = w / num_clips;
  const entity_id id = get_next_entity_id();
  const double dx = x;
  const double dy = y;
  sprites[id] = {is_animating, 0, num_clips, t, {0, 0, w, h}, {x, y, w, h}};
  transforms[id] = {dx, dy, 0, 0, 0, 1};
  entities.push_back(id);
  return id;
}

void spawn_skull(const int x, const int y) {
  if (player_id == -1) {
    const string key = "skull";
    const bool is_anim = false;
    const int num_frames = 2;
    const entity_id id = spawn_entity(key, is_anim, num_frames, x, y);
    inputs[id] = true;
    player_id = id;
    entity_types[id] = ENTITY_TYPE_PLAYER;
  }
}

void spawn_soulshard(const int x, const int y) {
  const string key = "soulshard";
  const bool is_anim = true;
  const int num_frames = 8;
  const entity_id id = spawn_entity("soulshard", is_anim, num_frames, x, y);
  const double vx = -1.0;
  transforms[id].vx = vx;
  is_soulshard[id] = true;
  entity_types[id] = ENTITY_TYPE_SOULSHARD;
}

void handle_knife_charge_decrement() {
  knife_charge--;
  if (knife_charge < 0) {
    knife_charge = 0;
  }
}

void handle_knife_charge_rotation(const entity_id id) {
  if (knife_charge > 0) {
    is_rotating[id] = true;
    rotation_speeds[id] = 5.0 * knife_charge;
  }
}

// void spawn_knives() {
//  always spawn at least one knife
// spawn_knife();

// const int num_additional_knives =
//     powerups_collected[POWERUP_TYPE_KNIFE_EXTRA];
// for (int i = 0; i < num_additional_knives; i++) {
//   spawn_knife_no_cooldown_no_count_check();
// }
//}

void spawn_knife_no_cooldown_no_count_check() {
  string key = "knife";
  if (knife_charge >= 2) {
    key = "knife-blue";
  }
  const int largeness = powerups_collected[POWERUP_TYPE_KNIFE_LARGENESS];
  const double scale = 1 + (0.1 * largeness);
  // const double scale = 1;
  const sprite_component player = sprites[player_id];
  const int padding_right = player.dest.w;
  SDL_Texture *t = textures[key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  const int padding_left = w * scale;
  const bool flipped = is_flipped[player_id];
  const double x =
      !flipped ? player.dest.x + padding_right : player.dest.x - padding_left;
  const double y = player.dest.y + player.dest.h / 4.0;
  const entity_id id = spawn_entity(key, false, 1, x, y);
  const double angle = flipped ? 180.0 : 0.0;
  double vx = current_knife_speed;

  if (flipped && knife_charge) {
    vx = -current_knife_speed * knife_charge - knife_charge;
  } else if (knife_charge) {
    vx = current_knife_speed * knife_charge + knife_charge;
  } else if (flipped) {
    vx = -current_knife_speed;
  }
  // const double vy = 0;
  // this adds a 'spray' effect to how knives fly out
  // we can create a powerup to alter the vy
  // const double vy = unit_distribution(rng_generator) * 2;
  const double vy = powerups_collected[POWERUP_TYPE_KNIFE_SPRAY] == 0
                        ? 0
                        : powerups_collected[POWERUP_TYPE_KNIFE_SPRAY] *
                              unit_distribution(rng_generator);
  transforms[id] = {x, y, vx, vy, angle, scale};
  is_knife[id] = true;
  entity_types[id] = ENTITY_TYPE_KNIFE;
  handle_knife_charge_rotation(id);
}

void spawn_knife() {
  if (!knife_cooldown && num_knives) {
    string key = "knife";
    if (knife_charge >= 2) {
      key = "knife-blue";
    }
    const int largeness = powerups_collected[POWERUP_TYPE_KNIFE_LARGENESS];
    const double scale = 1 + (0.1 * largeness);
    // const double scale = 1;
    const sprite_component player = sprites[player_id];
    const int padding_right = player.dest.w;
    SDL_Texture *t = textures[key];
    SDL_QueryTexture(t, NULL, NULL, &w, &h);
    const int padding_left = w * scale;
    const bool flipped = is_flipped[player_id];
    const double x =
        !flipped ? player.dest.x + padding_right : player.dest.x - padding_left;
    const double y = player.dest.y + player.dest.h / 4.0;
    const entity_id id = spawn_entity(key, false, 1, x, y);
    const double angle = flipped ? 180.0 : 0.0;
    double vx = current_knife_speed;

    if (flipped && knife_charge) {
      vx = -current_knife_speed * knife_charge - knife_charge;
    } else if (knife_charge) {
      vx = current_knife_speed * knife_charge + knife_charge;
    } else if (flipped) {
      vx = -current_knife_speed;
    }

    // const double vy = 0;
    // this adds a 'spray' effect to how knives fly out
    // we can create a powerup to alter the vy
    // const double vy = unit_distribution(rng_generator) * 2;
    const double vy = powerups_collected[POWERUP_TYPE_KNIFE_SPRAY] == 0
                          ? 0
                          : powerups_collected[POWERUP_TYPE_KNIFE_SPRAY] *
                                unit_distribution(rng_generator);

    transforms[id] = {x, y, vx, vy, angle, scale};
    is_knife[id] = true;
    entity_types[id] = ENTITY_TYPE_KNIFE;
    handle_knife_charge_rotation(id);
    knife_cooldown = current_knife_cooldown;
    num_knives_fired++;
    num_knives--;
    handle_knife_charge_decrement();
    const int num_additional_knives =
        powerups_collected[POWERUP_TYPE_KNIFE_EXTRA];

    for (int i = 0; i < num_additional_knives; i++) {
      spawn_knife_no_cooldown_no_count_check();
    }
  }
}

// void spawn_eyeball() {
//   const int x = config["target_texture_width"] - w;
//   const int y = rand() % (config["target_texture_height"] - h);
//   const entity_id id = spawn_entity("eyeball", true, 18, x, y);
//   const double vy = 0.0;
//   const double vx = eyeball_vx_distribution(rng_generator);
//   const double angle = 0.0;
//   const double scale = 1.0;
//   const double dx = x;
//   const double dy = y;
//   transforms[id] = {dx, dy, vx, vy, angle, scale};
//   entity_types[id] = ENTITY_TYPE_ENEMY;
//   is_collidable[id] = true;
//   is_enemy[id] = true;
// }

void spawn_bat_group(const double x, const double y, const double scale,
                     const int number) {
  assert(number > 0);
  assert(number < 10);
  string key = "bat";
  SDL_Texture *t = textures[key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  const int right = config["target_texture_width"] + w;
  const int bottom = config["target_texture_height"] - h * scale;
  assert(x > 0 && x < right);
  assert(y > 0 && y < bottom);
  double tmp_y = y;
  double vy = 0;
  shuffle(bat_vx_vec.begin(), bat_vx_vec.end(), g);
  shuffle(bat_y_vec.begin(), bat_y_vec.end(), g);
  for (int i = 0; i < number; i++) {
    double vx = bat_vx_vec[i];
    tmp_y = bat_y_vec[i];
    spawn_bat(x, tmp_y, vx, vy, scale);
  }
}

void spawn_bat(const double x, const double y, const double vx, const double vy,
               const double scale) {
  assert(x > 0);
  assert(y >= 0);
  assert(y < config["target_texture_height"]);
  const string key = "bat";
  SDL_QueryTexture(textures[key], NULL, NULL, &w, &h);
  const entity_id id = spawn_entity(key, true, 2, x, y);
  const double angle = 0.0;
  transforms[id] = {x, y, vx, vy, angle, scale};
  is_collidable[id] = true;
  is_enemy[id] = true;
  entity_types[id] = ENTITY_TYPE_ENEMY;
  enemy_types[id] = ENEMY_TYPE_BAT;
}

void spawn_generator(enemy_type type, bool active, int group, int cooldown,
                     int cooldown_reduction) {
  if (type > ENEMY_TYPE_COUNT) {
    return;
  }
  if (cooldown < 1) {
    return;
  }
  if (group < 1) {
    return;
  }

  for (auto kv : generators) {
    const generator_component generator = kv.second;
    if (generator.type == type) {
      return;
    }
  }
  const entity_id id = get_next_entity_id();
  generators[id] = {type, active, group, cooldown, cooldown_reduction};
  is_generator[id] = true;
  entity_types[id] = ENTITY_TYPE_GENERATOR;
  entities.push_back(id);
}

void spawn_powerup() {
  powerup_type poweruptype;
  poweruptype = (powerup_type)(rand() % POWERUP_TYPE_COUNT);

  // omit hearts from spawning if player health is max
  if (player_health == player_max_health) {
    while (poweruptype == POWERUP_TYPE_HEART) {
      poweruptype = (powerup_type)(rand() % POWERUP_TYPE_COUNT);
    }
  }

  string key = "powerup";
  SDL_Texture *t = textures[key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  entity_id id = -1;
  int num_frames = 1;
  bool is_anim = false;
  double x = config["target_texture_width"] + w;
  double y = texture_height_distribution(rng_generator) - h * 2;
  double angle = 0.0;
  switch (poweruptype) {
  case POWERUP_TYPE_KNIFE_LARGENESS:
    key = "powerup-knife-largeness";
    break;
  case POWERUP_TYPE_KNIFE_COOLDOWN:
    key = "powerup-knife-cooldown";
    break;
  case POWERUP_TYPE_KNIFE_SPEED:
    key = "powerup-knife-speed";
    break;
  case POWERUP_TYPE_HEART:
    key = "powerup-heart";
    num_frames = 6;
    is_anim = true;
    break;
  case POWERUP_TYPE_KNIFE_QUANTITY:
    key = "knife";
    angle = 90.0;
    break;
  case POWERUP_TYPE_KNIFE_SPRAY:
    key = "powerup-knife-spray";
    angle = 90.0;
    break;
  case POWERUP_TYPE_KNIFE_EXTRA:
    key = "powerup-knife-extra";
    angle = 270.0;
    break;
  case POWERUP_TYPE_SKULL_SPEED:
    key = "powerup-skull-speed";
    // current_player_speed += 2;
    break;

  case POWERUP_TYPE_MAGNETISM_THRESHOLD:
    key = "powerup-magnetism-threshold";
    num_frames = 8;
    is_anim = true;
    // t = textures[key];
    // SDL_QueryTexture(t, NULL, NULL, &w, &h);
    break;
  default:
    break;
  }

  id = spawn_entity(key, is_anim, num_frames, x, y);
  transforms[id].vx = DEFAULT_POWERUP_VX;
  transforms[id].angle = angle;
  is_powerup[id] = true;
  powerup_types[id] = poweruptype;
  entity_types[id] = ENTITY_TYPE_ITEM;
}

void spawn_blood_pixels(const int x, const int y, const int n) {
  // mPrint("spawn_blood_pixels: " + to_string(x) + ", " + to_string(y) + ", " +
  //        to_string(n));
  const string key = "blood-pixel";
  SDL_Texture *t = textures[key];
  // custom width/height defined in config/textures.json
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  const bool is_animating = false;
  const int num_clips = 1;
  const double dx = x;
  const double dy = y;
  for (int i = 0; i < n; i++) {
    entity_id id = get_next_entity_id();
    sprites[id] = {is_animating, 0, num_clips, t, {0, 0, w, h}, {x, y, w, h}};
    transforms[id] = {dx, dy,
                      // blood_velocity_negative_distribution(rng_generator),
                      blood_velocity_distribution(rng_generator),
                      blood_velocity_distribution(rng_generator), 0, 1};
    is_blood_pixel[id] = true;
    blood_pixel_lifetime[id] = 120;
    entity_types[id] = ENTITY_TYPE_PARTICLE;
    entities.push_back(id);
  }
}
