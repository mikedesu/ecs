#include "SDL_handler.h"
#include "entity_id.h"
#include "mPrint.h"
#include "sprite_component.h"
#include "transform_component.h"
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

using std::default_random_engine;
using std::string;
using std::to_string;
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
extern unordered_map<entity_id, bool> is_rotating;
extern int target_texture_width;
extern int target_texture_height;
extern default_random_engine rng_generator;
extern uniform_real_distribution<double> eyeball_vx_distribution;

extern entity_id get_next_entity_id();

void spawn_coin(int x, int y) {
  const int num_clips = 4;
  bool is_animating = true;
  SDL_QueryTexture(textures["coin"], NULL, NULL, &w, &h);
  w = w / num_clips;
  entity_id id = get_next_entity_id();
  mPrint("Spawning coin with id: " + to_string(id));
  // double x = (target_texture_width - w);
  // double y = (rand() % (target_texture_height - h));
  double vy = 0.0;
  double vx = -1.0;
  double angle = 0.0;
  sprites[id] = {is_animating, 0,
                 num_clips,    textures["coin"],
                 {0, 0, w, h}, {(int)x, (int)y, w, h}};
  transforms[id] = {(double)x, (double)y, vx, vy, angle};
  // is_collidable[id] = true;
  // is_enemy[id] = false;
  is_coin[id] = true;
  entities.push_back(id);
}
