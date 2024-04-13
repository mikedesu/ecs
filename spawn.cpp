#include "SDL_handler.h"
#include "entity_id.h"
#include "generator_component.h"
#include "powerup_type.h"
#include "sprite_component.h"
#include "transform_component.h"
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

using std::default_random_engine;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

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
extern int target_texture_width;
extern int target_texture_height;
extern int player_money;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> eyeball_vx_distribution;
extern uniform_real_distribution<double> texture_height_distribution;
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

entity_id spawn_entity(string key, bool is_animating, int num_clips, int x,
                       int y) {
  SDL_Texture *t = textures[key];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  w = w / num_clips;
  entity_id id = get_next_entity_id();
  sprites[id] = {is_animating, 0, num_clips, t, {0, 0, w, h}, {x, y, w, h}};
  transforms[id] = {(double)x, (double)y, 0, 0, 0, 1};
  entities.push_back(id);
  return id;
}

void spawn_skull() {
  if (player_id == -1) {
    entity_id id = spawn_entity("skull", false, 2, 0, 0);
    inputs[id] = true;
    player_id = id;
  }
}

void spawn_soulshard(int x, int y) {
  entity_id id = spawn_entity("soulshard", true, 8, x, y);
  is_soulshard[id] = true;
  transforms[id].vx = -1.0;
}

void spawn_knife() {
  if (!knife_cooldown && num_knives) {
    const int padding_right = 4;
    const int padding_left = 20;
    sprite_component player = sprites[player_id];
    SDL_Texture *t = textures["knife"];
    SDL_QueryTexture(t, NULL, NULL, &w, &h);

    double x = !is_flipped[player_id]
                   ? player.dest.x + player.dest.w + padding_right
                   : player.dest.x - w - padding_left;
    double y = player.dest.y + player.dest.h / 4.0;
    string key = "knife";
    if (knife_charge >= 2) {
      key = "knife-blue";
    }
    entity_id id = spawn_entity(key, false, 1, x, y);
    // entity_id id = spawn_entity("knife", false, 1, x, y);
    double angle = is_flipped[player_id] ? 180.0 : 0.0;
    double vx =
        is_flipped[player_id] ? -current_knife_speed : current_knife_speed;
    double vy = 0;
    int largeness = powerups_collected[POWERUP_TYPE_KNIFE_LARGENESS];
    double scale = 1.0 + (0.1 * largeness);
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
  int x = target_texture_width - w;
  int y = rand() % (target_texture_height - h);
  entity_id id = spawn_entity("eyeball", true, 18, x, y);
  double vy = 0.0;
  double vx = eyeball_vx_distribution(rng_generator);
  double angle = 0.0;
  double scale = 1.0;
  double dx = x;
  double dy = y;
  transforms[id] = {dx, dy, vx, vy, angle, scale};
  is_collidable[id] = true;
  is_enemy[id] = true;
}

void spawn_bat() {
  int x = target_texture_width - w;
  int y = rand() % (target_texture_height - (h * 2));
  entity_id id = spawn_entity("bat", true, 2, x, y);
  double vy = 0.0;
  double vx = eyeball_vx_distribution(rng_generator);
  double angle = 0.0;
  double scale = 1.0;
  double dx = x;
  double dy = y;
  transforms[id] = {dx, dy, vx, vy, angle, scale};
  is_collidable[id] = true;
  is_enemy[id] = true;
  enemy_types[id] = ENEMY_TYPE_BAT;
}

void spawn_generator(enemy_type type, bool active, int cooldown) {
  if (type > ENEMY_TYPE_COUNT) {
    return;
  }
  if (cooldown < 1) {
    return;
  }
  for (auto kv : generators) {
    generator_component generator = kv.second;
    if (generator.type == type) {
      return;
    }
  }
  entity_id id = get_next_entity_id();
  generators[id] = {type, active, cooldown};
  is_generator[id] = true;
  entities.push_back(id);
}

void spawn_powerup() {
  powerup_type poweruptype = (powerup_type)(rand() % POWERUP_TYPE_COUNT);
  SDL_Texture *t = textures["powerup"];
  SDL_QueryTexture(t, NULL, NULL, &w, &h);
  entity_id id = -1;
  int x = target_texture_width - w;
  int y = texture_height_distribution(rng_generator) - h;
  switch (poweruptype) {
  case POWERUP_TYPE_KNIFE_LARGENESS:
    id = spawn_entity("powerup", false, 1, x, y);
    break;
  case POWERUP_TYPE_KNIFE_COOLDOWN:
    id = spawn_entity("powerup", false, 1, x, y);
    break;
  case POWERUP_TYPE_KNIFE_QUANTITY:
    id = spawn_entity("knife", false, 1, x, y);
    transforms[id].angle = 90.0;
    break;
  default:
    break;
  }
  transforms[id].vx = -1.0;
  is_powerup[id] = true;
  is_rotating[id] = true;
  powerup_types[id] = poweruptype;
}
