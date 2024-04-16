
#include "SDL_handler.h"
#include "components.h"
#include "entity_id.h"
#include "gameconfig.h"
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

extern gameconfig config;
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

void bg_spawn_moon() {
  const int num_clips = 1;
  bool is_animating = false;
  int src_x = 0;
  int src_y = 0;
  double vx = -0.2;
  double vy = 0;
  double angle = 0.0;
  double scale = 1.0;
  SDL_Texture *t = textures["moon"];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  int x = config.target_texture_width;
  int y = 0;
  double dest_x = x;
  double dest_y = y;
  w = w / num_clips;
  entity_id id = get_next_entity_id();
  bg_sprites[id] = {is_animating,         0,           num_clips, t,
                    {src_x, src_y, w, h}, {x, y, w, h}};
  bg_transforms[id] = {dest_x, dest_y, vx, vy, angle, scale};
  entities.push_back(id);
}

void bg_init() {
  bg_spawn_moon();
  // bg_spawn_stars();
  // bg_spawn_clouds();
  // bg_spawn_mountains();
  // bg_spawn_trees();
  // bg_spawn_grass();
}
