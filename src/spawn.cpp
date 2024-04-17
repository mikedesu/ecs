#include "SDL_handler.h"
#include "components.h"
#include "entity_id.h"
#include "mPrint.h"
#include "powerup_type.h"
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

using std::default_random_engine;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

extern unordered_map<string, size_t> config;
extern entity_id player_id;
extern unordered_map<entity_id, bool> inputs;
extern unordered_map<string, SDL_Texture *> textures;
extern int w;
extern int h;
extern int knife_cooldown;
extern int current_knife_speed;
extern int current_knife_cooldown;
extern int num_knives;
extern int knife_charge;
extern int max_num_knives;
extern int num_knives_fired;
extern int player_money;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> eyeball_vx_distribution;
extern uniform_real_distribution<double> texture_height_distribution;
extern uniform_real_distribution<double> blood_velocity_positive_distribution;
extern uniform_real_distribution<double> blood_velocity_negative_distribution;
extern uniform_real_distribution<double> blood_velocity_distribution;

extern unordered_map<entity_id, bool> is_blood_pixel;
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

void spawn_skull() {
  if (player_id == -1) {
    const entity_id id = spawn_entity("skull", false, 2, 0, 0);
    inputs[id] = true;
    player_id = id;
  }
}

void spawn_soulshard(const int x, const int y) {
  const entity_id id = spawn_entity("soulshard", true, 8, x, y);
  is_soulshard[id] = true;
  transforms[id].vx = -1.0;
}

void spawn_knife() {
  if (!knife_cooldown && num_knives) {
    string key = knife_charge >= 2 ? "knife-blue" : "knife";
    const int largeness = powerups_collected[POWERUP_TYPE_KNIFE_LARGENESS];
    const double scale = 1.0 + (0.1 * largeness);
    const sprite_component player = sprites[player_id];
    const int padding_right = player.dest.w + 4;
    SDL_Texture *t = textures[key];
    SDL_QueryTexture(t, NULL, NULL, &w, &h);
    const int padding_left = w * scale;
    const bool flipped = is_flipped[player_id];
    const double x =
        !flipped ? player.dest.x + padding_right : player.dest.x - padding_left;
    const double y = player.dest.y + player.dest.h / 4.0;
    const entity_id id = spawn_entity(key, false, 1, x, y);
    const double angle = flipped ? 180.0 : 0.0;
    const double vx = flipped ? -current_knife_speed : current_knife_speed;
    const double vy = 0;
    transforms[id] = {x, y, vx, vy, angle, scale};
    is_knife[id] = true;
    is_rotating[id] = knife_charge > 0;
    if (knife_charge > 0) {
      rotation_speeds[id] = 5.0 * knife_charge;
    }
    knife_cooldown = current_knife_cooldown;
    num_knives_fired++;
    num_knives--;
    if (knife_charge > 0) {
      knife_charge--;
      if (knife_charge < 0) {
        knife_charge = 0;
      }
    }
  }
}

void spawn_eyeball() {
  const int x = config["target_texture_width"] - w;
  const int y = rand() % (config["target_texture_height"] - h);
  const entity_id id = spawn_entity("eyeball", true, 18, x, y);
  const double vy = 0.0;
  const double vx = eyeball_vx_distribution(rng_generator);
  const double angle = 0.0;
  const double scale = 1.0;
  const double dx = x;
  const double dy = y;
  transforms[id] = {dx, dy, vx, vy, angle, scale};
  is_collidable[id] = true;
  is_enemy[id] = true;
}

void spawn_bat() {
  const int x = config["target_texture_width"] - w;
  const int y = rand() % (config["target_texture_height"] - (h * 2));
  const entity_id id = spawn_entity("bat", true, 2, x, y);
  const double vy = 0.0;
  const double vx = eyeball_vx_distribution(rng_generator);
  const double angle = 0.0;
  const double scale = 1.0;
  const double dx = x;
  const double dy = y;
  transforms[id] = {dx, dy, vx, vy, angle, scale};
  is_collidable[id] = true;
  is_enemy[id] = true;
  enemy_types[id] = ENEMY_TYPE_BAT;
}

void spawn_generator(enemy_type type, bool active, int cooldown,
                     int cooldown_reduction) {
  if (type > ENEMY_TYPE_COUNT) {
    return;
  }
  if (cooldown < 1) {
    return;
  }
  for (auto kv : generators) {
    const generator_component generator = kv.second;
    if (generator.type == type) {
      return;
    }
  }
  const entity_id id = get_next_entity_id();
  generators[id] = {type, active, cooldown, cooldown_reduction};
  is_generator[id] = true;
  entities.push_back(id);
}

void spawn_powerup() {
  const powerup_type poweruptype = (powerup_type)(rand() % POWERUP_TYPE_COUNT);
  SDL_Texture *t = textures["powerup"];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  entity_id id = -1;
  const int x = config["target_texture_width"] - w;
  const int y = texture_height_distribution(rng_generator) - h;
  switch (poweruptype) {
  case POWERUP_TYPE_KNIFE_LARGENESS:
    id = spawn_entity("powerup-knife-largeness", false, 1, x, y);
    is_rotating[id] = true;
    break;
  case POWERUP_TYPE_KNIFE_COOLDOWN:
    id = spawn_entity("powerup-knife-cooldown", false, 1, x, y);
    is_rotating[id] = true;
    break;
  case POWERUP_TYPE_KNIFE_SPEED:
    id = spawn_entity("powerup-knife-speed", false, 1, x, y);
    is_rotating[id] = true;
    break;
  case POWERUP_TYPE_HEART:
    id = spawn_entity("powerup-heart", true, 6, x, y);
    break;
  case POWERUP_TYPE_KNIFE_QUANTITY:
    id = spawn_entity("knife", false, 1, x, y);
    is_rotating[id] = true;
    transforms[id].angle = 90.0;
    break;
  default:
    break;
  }
  transforms[id].vx = -1.0;
  is_powerup[id] = true;
  // is_rotating[id] = true;
  powerup_types[id] = poweruptype;
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
    entities.push_back(id);
  }
}

void spawn_blood_pixel(int x, int y) {
  const string key = "blood-pixel";
  SDL_Texture *t = textures[key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  const bool is_animating = false;
  const int num_clips = 1;
  const entity_id id = get_next_entity_id();
  sprites[id] = {is_animating, 0, num_clips, t, {0, 0, w, h}, {x, y, w, h}};
  transforms[id] = {(double)x,
                    (double)y,
                    blood_velocity_negative_distribution(rng_generator),
                    blood_velocity_distribution(rng_generator),
                    0,
                    1};
  entities.push_back(id);
  is_blood_pixel[id] = true;
}
