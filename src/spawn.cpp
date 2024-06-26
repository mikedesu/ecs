#include "SDL_handler.h"
#include "components.h"
#include "enemy_bullets.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "entity_type.h"
#include "mPrint.h"
#include "powerup_type.h"
#include <SDL_render.h>
#include <algorithm>
// #include <cassert>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#define DEFAULT_POWERUP_VX -1.0

using std::default_random_engine;
using std::map;
using std::mt19937;
using std::shuffle;
using std::string;
using std::to_string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

extern mt19937 g;
extern vector<int> bat_y_vec;
extern entity_id player_id;
extern int powerups_onscreen;
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
extern uniform_real_distribution<double> texture_height_distribution;
extern uniform_real_distribution<double> blood_velocity_positive_distribution;
extern uniform_real_distribution<double> blood_velocity_negative_distribution;
extern uniform_real_distribution<double> blood_velocity_distribution;
extern vector<double> bat_vx_vec;
extern map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, int> knife_charges;
extern unordered_map<entity_id, int> explosion_frames;
extern unordered_map<entity_id, int> hitpoints;
extern unordered_map<string, int> num_clips;
extern unordered_map<string, size_t> config;
extern unordered_map<entity_id, bool> inputs;
extern unordered_map<string, SDL_Texture *> textures;
extern unordered_map<entity_id, entity_type> entity_types;
extern unordered_map<entity_id, int> blood_pixel_lifetime;
extern unordered_map<entity_id, powerup_type> powerup_types;
extern unordered_map<entity_id, enemy_type> enemy_types;
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
extern unordered_map<enemy_type, enemy_bullet_definition>
    enemy_bullet_definitions;
extern vector<entity_id> entities;
extern entity_id get_next_entity_id();

// function declarations
void spawn_generator(enemy_type type, bool active, int group, int cooldown,
                     int cooldown_reduction, int frame_begin, int spawn_count,
                     int hp, double scale, screen_position_t screen_position);
void spawn_eyeball(const double x, const double y, const double vx,
                   const double vy, const double scale, const int hp);
void spawn_soulshard(const int x, const int y);
void spawn_bat(const double x, const double y, const double vx, const double vy,
               const double scale, const int hp);
void spawn_goblin(const double x, const double y, const double vx,
                  const double vy, const double scale, const int hp,
                  const enemy_type type);
entity_id spawn_entity(const string key, const bool is_animating,
                       const int numclips, const int x, const int y,
                       const double scale);
void spawn_block(const int x, const int y, const int w, const int h);
void spawn_goblin_bullet(entity_id id);
void spawn_knife();
void spawn_skull(const int x, const int y);
void spawn_powerup();
void spawn_small_explosion(const int x, const int y);
void spawn_blood_pixels(const int x, const int y, const int n);

// function implementations
entity_id spawn_entity(const string key, const bool is_animating,
                       const int numclips, const int x, const int y,
                       const double scale) {
  const string dmg_key = key + "-dmg";
  SDL_Texture *t = textures[key];
  SDL_Texture *t2 = textures[dmg_key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  w = w / numclips;
  const entity_id id = get_next_entity_id();
  const double dx = x;
  const double dy = y;
  // const double scale = 1.0;
  sprites[id] = {
      is_animating, 0, numclips,     t,
      t2,           0, {0, 0, w, h}, {x, y, (int)scale * w, (int)scale * h}};
  transforms[id] = {dx, dy, 0, 0, 0, scale};
  entities.push_back(id);
  return id;
}

void spawn_goblin(const double x, const double y, const double vx,
                  const double vy, const double scale, const int hp,
                  const enemy_type type) {

  //  mPrint("spawn_goblin: " + to_string(x) + " " + to_string(y) + " " +
  //         to_string(vx) + " " + to_string(vy) + " " + to_string(scale) + " "
  //         + to_string(hp) + " " + to_string(type));

  if (type != ENEMY_TYPE_GOBLIN && type != ENEMY_TYPE_GOBLIN_2) {
    // if (type != ENEMY_TYPE_GOBLIN_2) {
    mPrint("ERROR: spawn_goblin: invalid enemy type");
    return;
  }

  const string key = type == ENEMY_TYPE_GOBLIN     ? "goblin"
                     : type == ENEMY_TYPE_GOBLIN_2 ? "goblin2"
                                                   : "goblin";

  SDL_QueryTexture(textures[key], NULL, NULL, &w, &h);
  const int numclips = num_clips[key];
  // const entity_id id = spawn_entity(key, true, numclips, x, y);
  const entity_id id = spawn_entity(key, true, numclips, x, y, scale);
  const double angle = 0.0;
  transforms[id] = {x, y, vx, vy, angle, scale};
  is_collidable[id] = true;
  is_enemy[id] = true;
  entity_types[id] = ENTITY_TYPE_ENEMY;

  // enemy_types[id] = ENEMY_TYPE_GOBLIN;
  enemy_types[id] = type;

  // hitpoints[id] = 1;
  hitpoints[id] = hp;
}

void spawn_bat(const double x, const double y, const double vx, const double vy,
               const double scale, const int hp) {
  const string key = "bat";
  SDL_QueryTexture(textures[key], NULL, NULL, &w, &h);
  const int numclips = num_clips[key];
  // const entity_id id = spawn_entity(key, true, numclips, x, y);
  const entity_id id = spawn_entity(key, true, numclips, x, y, scale);
  const double angle = 0.0;
  transforms[id] = {x, y, vx, vy, angle, scale};
  is_collidable[id] = true;
  is_enemy[id] = true;
  entity_types[id] = ENTITY_TYPE_ENEMY;
  enemy_types[id] = ENEMY_TYPE_BAT;
  // hitpoints[id] = 1;
  hitpoints[id] = hp;
}

void spawn_goblin_bullet(entity_id id) {
  // the key can be used to pull a bullet definition
  string key = "goblin-bullet";
  const double scale = 1.0;
  const sprite_component goblin = sprites[id];
  const double x = goblin.dest.x + goblin.dest.w / 2.0;
  const double y = goblin.dest.y;

  double vx = 0;
  double vy = 0;

  enemy_type type = enemy_types[id];

  // mPrint("bullet type: " + to_string(type));

  enemy_bullet_definition def = enemy_bullet_definitions[type];

  switch (def.movement) {
  case ENEMY_BULLET_MOVEMENT_UP:
    vx = 0;
    vy = -def.speed;
    break;
  case ENEMY_BULLET_MOVEMENT_TOWARDS_PLAYER:
    // experiment time
    // lets make the bullets fire at the player
    // to do this, we need the player position
    const int px = sprites[player_id].dest.x;
    const int py = sprites[player_id].dest.y;
    // calculate the angle between the player and the goblin
    const double angle = atan2(py - y, px - x);
    vx = cos(angle) * def.speed;
    vy = sin(angle) * def.speed;
    break;
  }

  // experiment time
  // lets make the bullets fire at the player
  // to do this, we need the player position
  // const int px = sprites[player_id].dest.x;
  // const int py = sprites[player_id].dest.y;
  // calculate the angle between the player and the goblin
  // const double angle = atan2(py - y, px - x);
  // double vx = cos(angle) * 4.0;
  // double vy = sin(angle) * 4.0;

  // const double vx = 0;
  // const double vy = -1.0;

  // entity_id bullet_id = spawn_entity(key, false, 1, x, y);
  entity_id bullet_id = spawn_entity(key, false, 1, x, y, scale);
  transforms[bullet_id] = {x, y, vx, vy, 0, scale};
  entity_types[bullet_id] = ENTITY_TYPE_ENEMY_BULLET;
  is_rotating[bullet_id] = true;
}

// REFACTOR
void spawn_knife() {
  if (!knife_cooldown && num_knives) {
    string key = "knife";
    if (knife_charge >= 2) {
      key = "knife-blue";
    }
    const int largeness = powerups_collected[POWERUP_TYPE_KNIFE_LARGENESS];
    const double scale = 1 + (0.1 * largeness);
    const sprite_component player = sprites[player_id];
    const int padding_right = player.dest.w;
    SDL_Texture *t = textures[key];
    SDL_QueryTexture(t, NULL, NULL, &w, &h);
    const int padding_left = w * scale;
    const bool flipped = is_flipped[player_id];
    const double x =
        !flipped ? player.dest.x + padding_right : player.dest.x - padding_left;
    const double y = player.dest.y + player.dest.h / 4.0;
    const double angle = flipped ? 180.0 : 0.0;
    double vx = current_knife_speed;
    if (flipped && knife_charge) {
      vx = -current_knife_speed * knife_charge;
    } else if (knife_charge) {
      vx = current_knife_speed * knife_charge;
    } else if (flipped) {
      vx = -current_knife_speed;
    }
    double vy = 0;
    const int sprayups_collected = powerups_collected[POWERUP_TYPE_KNIFE_SPRAY];
    // limit the number of sprayups to 5
    // sprayup affects the knife amplitude
    if (sprayups_collected >= 1 && sprayups_collected <= 5) {
      vy = sprayups_collected * unit_distribution(rng_generator);
    } else if (sprayups_collected > 5) {
      vy = 5 * unit_distribution(rng_generator);
    }
    entity_id id = -1;
    // limit the number of knifeextras to 4
    const int extraknives = powerups_collected[POWERUP_TYPE_KNIFE_EXTRA];
    int total_knives = 1;
    if (extraknives < 4) {
      total_knives = 1 + extraknives;
    } else {
      total_knives = 4;
    }
    const bool do_spray = powerups_collected[POWERUP_TYPE_KNIFE_SPRAY] > 0;
    int i = 0;
    const int numclips = num_clips[key];
    do {
      // id = spawn_entity(key, false, numclips, x, y);
      id = spawn_entity(key, false, numclips, x, y, scale);
      vy = !do_spray ? 0
                     : powerups_collected[POWERUP_TYPE_KNIFE_SPRAY] *
                           unit_distribution(rng_generator);
      transforms[id] = {x, y, vx, vy, angle, scale};
      is_knife[id] = true;
      entity_types[id] = ENTITY_TYPE_KNIFE;
      if (knife_charge > 0) {
        is_rotating[id] = true;
        rotation_speeds[id] = 5.0 * knife_charge;
      }
      knife_cooldown = current_knife_cooldown;
      num_knives_fired++;
      if (i == 0) {
        knife_charges[id] = knife_charge;
        knife_charge--;
        if (knife_charge < 0) {
          knife_charge = 0;
        }
        num_knives--;
      }
      i++;
    } while (i < total_knives);
  }
}

void spawn_small_explosion(const int x, const int y) {
  const string key = "explosion-2-small";
  SDL_Texture *t = textures[key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  const bool is_anim = true;
  const int numclips = num_clips[key];
  const double dx = x;
  const double dy = y;
  const double scale = 1.0;
  // entity_id id = spawn_entity(key, is_anim, numclips, x, y);
  entity_id id = spawn_entity(key, is_anim, numclips, x, y, scale);
  entity_types[id] = ENTITY_TYPE_EXPLOSION;
  transforms[id] = {dx, dy, 0, 0, 0, 2.0};
  explosion_frames[id] = numclips * 6;
  entities.push_back(id);
}

void spawn_skull(const int x, const int y) {
  if (player_id == -1) {
    const string key = "skull";
    const bool is_anim = false;
    const int num_frames = num_clips[key];
    const double scale = 1.0;
    // const entity_id id = spawn_entity(key, is_anim, num_frames, x, y);
    const entity_id id = spawn_entity(key, is_anim, num_frames, x, y, scale);
    inputs[id] = true;
    player_id = id;
    entity_types[id] = ENTITY_TYPE_PLAYER;
  }
}

void spawn_soulshard(const int x, const int y) {
  const string key = "soulshard";
  const bool is_anim = true;
  const int num_frames = num_clips[key];
  // const entity_id id = spawn_entity("soulshard", is_anim, num_frames, x, y);
  const double scale = 1.0;
  const entity_id id =
      spawn_entity("soulshard", is_anim, num_frames, x, y, scale);
  const double vx = -1.0;
  transforms[id].vx = vx;
  is_soulshard[id] = true;
  entity_types[id] = ENTITY_TYPE_SOULSHARD;
}

void spawn_bats(const double x, const double y, const double scale,
                const double vx, const double vy, const int number,
                const int hp) {
  if (number < 1 || number > 10) {
    return;
  }
  string key = "bat";
  SDL_Texture *t = textures[key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  w = w / num_clips[key];
  double tmp_y = y;
  shuffle(bat_vx_vec.begin(), bat_vx_vec.end(), g);
  shuffle(bat_y_vec.begin(), bat_y_vec.end(), g);
  for (int i = 0; i < number; i++) {
    double myvx = bat_vx_vec[i] * vx;
    tmp_y = bat_y_vec[i];
    // spawn_bat(x, tmp_y, myvx, vy, scale);
    spawn_bat(x, tmp_y, myvx, vy, scale, hp);
  }
}

void spawn_eyeball(const double x, const double y, const double vx,
                   const double vy, const double scale, const int hp) {
  mPrint("spawn_eyeball: " + to_string(x) + " " + to_string(y) + " " +
         to_string(vx) + " " + to_string(vy) + " " + to_string(scale) + " " +
         to_string(hp));
  const string key = "eyeball";
  SDL_QueryTexture(textures[key], NULL, NULL, &w, &h);
  const int numclips = num_clips[key];
  // const entity_id id = spawn_entity(key, true, numclips, x, y);
  const entity_id id = spawn_entity(key, true, numclips, x, y, scale);
  const double angle = 0.0;
  transforms[id] = {x, y, vx, vy, angle, scale};
  is_collidable[id] = true;
  is_enemy[id] = true;
  entity_types[id] = ENTITY_TYPE_ENEMY;
  enemy_types[id] = ENEMY_TYPE_EYEBALL;
  // hitpoints[id] = (int)scale > 1 ? scale : 1;
  hitpoints[id] = hp;
}

void spawn_generator(enemy_type type, bool active, int group, int cooldown,
                     int cooldown_reduction, int frame_begin, int spawn_count,
                     int hp, double scale, screen_position_t screen_position) {

  if (type > ENEMY_TYPE_COUNT) {
    return;
  }
  if (cooldown < 1) {
    return;
  }
  if (group < 1) {
    return;
  }
  if (frame_begin < 0) {
    return;
  }
  if (spawn_count == 0) {
    return;
  }
  if (hp < 1) {
    return;
  }

  // mPrint("spawn_generator: " + to_string(type) + " " + to_string(active) + "
  // " +
  //        to_string(group) + " " + to_string(cooldown) + " " +
  //        to_string(cooldown_reduction) + " " + to_string(frame_begin) + " " +
  //        to_string(spawn_count) + " " + to_string(hp));

  const entity_id id = get_next_entity_id();
  generators[id] = {
      type,        active,      group, cooldown, cooldown_reduction,
      frame_begin, spawn_count, hp,    scale,    screen_position};
  is_generator[id] = true;
  entity_types[id] = ENTITY_TYPE_GENERATOR;
  entities.push_back(id);
}

void spawn_powerup() {
  powerup_type poweruptype;
  poweruptype = (powerup_type)(rand() % POWERUP_TYPE_COUNT);
  // omit hearts from spawning if player health is max
  // this needs to be re-written to be more intelligent
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
  if (y < 0) {
    y = 0;
  }
  // y = 0;
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
    break;
  case POWERUP_TYPE_MAGNETISM_THRESHOLD:
    key = "powerup-magnetism-threshold";
    num_frames = 8;
    is_anim = true;
    break;
  default:
    break;
  }
  // id = spawn_entity(key, is_anim, num_frames, x, y);
  const double scale = 1.0;
  id = spawn_entity(key, is_anim, num_frames, x, y, scale);
  transforms[id].vx = DEFAULT_POWERUP_VX;
  transforms[id].angle = angle;
  is_powerup[id] = true;
  powerup_types[id] = poweruptype;
  entity_types[id] = ENTITY_TYPE_ITEM;
  powerups_onscreen++;
}

void spawn_blood_pixels(const int x, const int y, const int n) {
  const string key = "blood-pixel";
  SDL_Texture *t = textures[key];
  // custom width/height defined in config/textures.json
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  const bool is_animating = false;
  const int numclips = num_clips[key];
  const double dx = x;
  const double dy = y;
  // if we have too many blood pixels, dont bother spawning anymore
  for (int i = 0; i < n; i++) {
    entity_id id = get_next_entity_id();
    sprites[id] = {is_animating, 0, numclips,     t,
                   NULL,         0, {0, 0, w, h}, {x, y, w, h}};
    transforms[id] = {dx, dy,
                      // blood_velocity_negative_distribution(rng_generator),
                      // blood_velocity_distribution(rng_generator),
                      blood_velocity_positive_distribution(rng_generator),
                      blood_velocity_distribution(rng_generator), 0, 1};
    // is_blood_pixel[id] = true;
    blood_pixel_lifetime[id] = rand() % 120;
    entity_types[id] = ENTITY_TYPE_PARTICLE;
    entities.push_back(id);
  }
}

void spawn_block(const int x, const int y, const int w, const int h) {
  const string key = "block-01-small";
  // SDL_Texture *t = textures[key];
  // SDL_QueryTexture(t, NULL, NULL, &w, &h);
  const bool is_anim = false;
  const int numclips = num_clips[key];
  const double dx = x;
  const double dy = y;
  const double scale = 1.0;
  entity_id id = spawn_entity(key, is_anim, numclips, x, y, scale);
  transforms[id] = {dx, dy, 0, 0, 0, scale};
  is_collidable[id] = true;
  entity_types[id] = ENTITY_TYPE_BLOCK;
}
