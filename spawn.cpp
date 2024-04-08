
#include "SDL_handler.h"
#include "entity_id.h"
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
extern int num_knives_fired;
extern int target_texture_width;
extern int target_texture_height;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> eyeball_vx_distribution;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, bool> is_coin;
extern unordered_map<entity_id, bool> is_flipped;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_rotating;
extern unordered_map<entity_id, bool> is_collidable;
extern unordered_map<entity_id, bool> is_enemy;
extern vector<entity_id> entities;

extern entity_id get_next_entity_id();

void spawn_skull() {
  if (player_id == -1) {
    const int num_clips = 2;
    bool is_animating = false;
    int src_x = 0;
    int src_y = 0;
    double dest_x = 0;
    double dest_y = 0;
    double vx = 0;
    double vy = 0;
    double angle = 0.0;
    double scale = 1.0;
    SDL_QueryTexture(textures["skull"], NULL, NULL, &w, &h);
    w = w / num_clips;
    entity_id id = get_next_entity_id();
    sprites[id] = {is_animating,
                   0,
                   num_clips,
                   textures["skull"],
                   {src_x, src_y, w, h},
                   {(int)dest_x, (int)dest_y, w, h}};
    transforms[id] = {dest_x, dest_y, vx, vy, angle, scale};
    inputs[id] = true;
    player_id = id;
    entities.push_back(id);
  }
}

void spawn_coin(int x, int y) {
  const int num_clips = 4;
  bool is_animating = true;
  SDL_QueryTexture(textures["coin"], NULL, NULL, &w, &h);
  w = w / num_clips;
  entity_id id = get_next_entity_id();
  double vy = 0.0;
  double vx = -1.0;
  double angle = 0.0;
  double scale = 1.0;
  double dx = x;
  double dy = y;
  sprites[id] = {is_animating, 0,           num_clips, textures["coin"],
                 {0, 0, w, h}, {x, y, w, h}};
  transforms[id] = {dx, dy, vx, vy, angle, scale};
  is_coin[id] = true;
  entities.push_back(id);
}

void spawn_knife() {
  if (!knife_cooldown) {
    const int num_clips = 1;
    const int padding = 16;
    bool is_animating = false;
    entity_id id = get_next_entity_id();
    sprite_component sprite = sprites[player_id];
    double x = sprite.dest.x + sprite.dest.w + padding;
    double y = sprite.dest.y + sprite.dest.h / 2.0 + 4;
    double angle = 0.0;
    double vx = current_knife_speed;
    double vy = 0;
    if (is_flipped[player_id]) {
      x = sprite.dest.x - padding;
      vx = -current_knife_speed;
      angle = 180.0;
    }
    SDL_QueryTexture(textures["knife"], NULL, NULL, &w, &h);
    w = w / num_clips;
    double scale = 1.2;
    sprites[id] = {is_animating, 0,           num_clips, textures["knife"],
                   {0, 0, w, h}, {0, 0, w, h}};
    transforms[id] = {x, y, vx, vy, angle, scale};
    is_knife[id] = true;
    is_rotating[id] = true;
    entities.push_back(id);
    knife_cooldown = current_knife_cooldown;
    num_knives_fired++;
  }
}

void spawn_eyeball() {
  const int num_clips = 18;
  bool is_animating = true;
  SDL_QueryTexture(textures["eyeball"], NULL, NULL, &w, &h);
  w = w / num_clips;
  entity_id id = get_next_entity_id();
  int x = (target_texture_width - w);
  int y = (rand() % (target_texture_height - h));
  double vy = 0.0;
  double vx = eyeball_vx_distribution(rng_generator);
  double angle = 0.0;
  double scale = 1.0;
  double dx = x;
  double dy = y;
  sprites[id] = {is_animating, 0,           num_clips, textures["eyeball"],
                 {0, 0, w, h}, {x, y, w, h}};
  transforms[id] = {dx, dy, vx, vy, angle, scale};
  is_collidable[id] = true;
  is_enemy[id] = true;
  entities.push_back(id);
}
