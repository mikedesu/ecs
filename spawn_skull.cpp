
#include "SDL_handler.h"
#include "entity_id.h"
#include "sprite_component.h"
#include "transform_component.h"
#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;

extern entity_id player_id;

extern unordered_map<entity_id, bool> inputs;
extern unordered_map<string, SDL_Texture *> textures;
extern int w, h;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, transform_component> transforms;
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
