
#include "SDL_handler.h"
#include "components.h"
#include "entity_id.h"
// #include "gameconfig.h"
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

// extern gameconfig config;
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
extern int max_num_knives;
extern int num_knives_fired;
extern int player_money;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> eyeball_vx_distribution;
extern uniform_real_distribution<double> texture_height_distribution;
extern unordered_map<entity_id, powerup_type> powerup_types;
extern unordered_map<entity_id, enemy_type> enemy_types;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, sprite_component> bg_sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, transform_component> bg_transforms;
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
extern vector<entity_id> entities;

extern entity_id get_next_entity_id();

void bg_spawn_candle() {
  const string key = "candle";
  const string target_texture_width_config_key = "target_texture_width";
  const string target_texture_height_config_key = "target_texture_height";
  const int num_clips = 3;
  const bool is_animating = true;
  const int src_x = 0;
  const int src_y = 0;
  const double vx = -1.0 * (rand() % 4);
  const double vy = 0;
  const double angle = 0.0;
  const double scale = rand() % 5 + 1;
  SDL_Texture *t = textures[key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  const int x = rand() % config[target_texture_width_config_key];
  const int y = config[target_texture_height_config_key] - h * scale;
  const double dest_x = x;
  const double dest_y = y;
  w = w / num_clips;
  const entity_id id = get_next_entity_id();
  bg_sprites[id] = {is_animating,         0,           num_clips, t,
                    {src_x, src_y, w, h}, {x, y, w, h}};
  bg_transforms[id] = {dest_x, dest_y, vx, vy, angle, scale};
  entities.push_back(id);
}

void bg_spawn_moon() {
  const string moon_texture_key = "moon";
  const string target_texture_width_config_key = "target_texture_width";
  const int num_clips = 1;
  const bool is_animating = false;
  const int src_x = 0;
  const int src_y = 0;
  const double vx = -0.2;
  const double vy = 0;
  const double angle = 0.0;
  const double scale = 1.0;
  SDL_Texture *t = textures[moon_texture_key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  const int x = config[target_texture_width_config_key];
  const int y = 0;
  const double dest_x = x;
  const double dest_y = y;
  w = w / num_clips;
  const entity_id id = get_next_entity_id();
  bg_sprites[id] = {is_animating,         0,           num_clips, t,
                    {src_x, src_y, w, h}, {x, y, w, h}};
  bg_transforms[id] = {dest_x, dest_y, vx, vy, angle, scale};
  entities.push_back(id);
}

void bg_init() {
  bg_spawn_moon();

  bg_spawn_candle();
  bg_spawn_candle();
  bg_spawn_candle();
  bg_spawn_candle();
  bg_spawn_candle();
  bg_spawn_candle();
  bg_spawn_candle();
  bg_spawn_candle();
  bg_spawn_candle();

  // bg_spawn_stars();
  // bg_spawn_clouds();
  // bg_spawn_mountains();
  // bg_spawn_trees();
  // bg_spawn_grass();
}
