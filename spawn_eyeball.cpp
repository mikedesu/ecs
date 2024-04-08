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
extern int w, h;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern vector<entity_id> entities;
extern unordered_map<entity_id, bool> is_collidable;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_coin;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_rotating;
extern int target_texture_width;
extern int target_texture_height;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> eyeball_vx_distribution;

extern entity_id get_next_entity_id();

void spawn_eyeball() {
  const int num_clips = 18;
  bool is_animating = true;
  SDL_QueryTexture(textures["eyeball"], NULL, NULL, &w, &h);
  w = w / num_clips;
  entity_id id = get_next_entity_id();
  double x = (target_texture_width - w);
  double y = (rand() % (target_texture_height - h));
  double vy = 0.0;
  double vx = eyeball_vx_distribution(rng_generator);
  // double vy = distribution(generator);
  double angle = 0.0;
  double scale = 1.0;
  sprites[id] = {is_animating, 0,
                 num_clips,    textures["eyeball"],
                 {0, 0, w, h}, {(int)x, (int)y, w, h}};
  transforms[id] = {x, y, vx, vy, angle, scale};
  is_collidable[id] = true;
  is_enemy[id] = true;
  // is_knife[id] = false;
  // is_coin[id] = false;
  entities.push_back(id);
}
