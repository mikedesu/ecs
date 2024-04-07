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

extern int knife_cooldown;
extern int current_knife_cooldown;
extern int current_knife_speed;
extern int w;
extern int h;
extern int num_knives_fired;
extern entity_id player_id;
extern unordered_map<string, SDL_Texture *> textures;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_collidable;
extern unordered_map<entity_id, bool> is_rotating;
extern unordered_map<entity_id, bool> is_flipped;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern vector<entity_id> entities;

extern entity_id get_next_entity_id();

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

    // mPrint("spawning knife with id " + std::to_string(id));
    SDL_QueryTexture(textures["knife"], NULL, NULL, &w, &h);
    w = w / num_clips;
    sprites[id] = {is_animating, 0,           num_clips, textures["knife"],
                   {0, 0, w, h}, {0, 0, w, h}};
    transforms[id] = {x, y, vx, vy, angle};
    is_knife[id] = true;
    is_enemy[id] = false;
    is_collidable[id] = true;
    is_rotating[id] = true;
    entities.push_back(id);
    knife_cooldown = current_knife_cooldown;
    num_knives_fired++;
  }
}
