#include "entity_id.h"
#include "sprite_component.h"
#include "transform_component.h"
#include "transform_pair.h"
#include <algorithm>
#include <functional>
#include <unordered_map>
// #include <utility>

using std::for_each;
using std::function;
using std::unordered_map;
// using std::pair;

extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, sprite_component> sprites;
extern entity_id player_id;
extern unordered_map<entity_id, bool> is_enemy;
extern unordered_map<entity_id, bool> is_knife;
extern unordered_map<entity_id, bool> is_coin;
extern unordered_map<entity_id, bool> is_marked_for_deletion;
extern int num_enemies_escaped;
extern int window_width;
extern int target_texture_width;
extern int target_texture_height;

function<void(transform_pair)> handle_transform = [](const transform_pair t) {
  entity_id id = t.first;
  transform_component transform = t.second;
  sprite_component sprite = sprites[id];

  transform.x += transform.vx;
  transform.y += transform.vy;

  if (id == player_id) {
    // bounds checking
    // player cannot move beyond texture

    if (transform.x < 0) {
      transform.x = 0;
    } else if (transform.x > target_texture_width - sprite.dest.w) {
      transform.x = target_texture_width - sprite.dest.w;
    }

    if (transform.y < 0) {
      transform.y = 0;
    } else if (transform.y > target_texture_height - sprite.dest.h) {
      transform.y = target_texture_height - sprite.dest.h;
    }
  }

  sprite.dest.x = transform.x;
  sprite.dest.y = transform.y;
  sprite.dest.w = sprite.src.w * transform.scale;
  sprite.dest.h = sprite.src.h * transform.scale;

  sprites[id] = sprite;

  if (id != player_id) {
    if (is_enemy[id]) {
      bool marked = transform.x < -sprite.src.w;
      is_marked_for_deletion[id] = marked;
      if (marked) {
        num_enemies_escaped++;
      }
    } else if (is_knife[id] || is_coin[id]) {
      is_marked_for_deletion[id] =
          transform.x < -sprite.src.w || transform.x > window_width;
    }
  }
  transforms[id] = transform;
};

void update_transform_components() {
  for_each(transforms.begin(), transforms.end(), handle_transform);
}
