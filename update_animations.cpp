#include "entity_id.h"
#include "sprite_component.h"
#include "sprite_pair.h"
#include <algorithm>
#include <functional>
#include <unordered_map>

using std::for_each;
using std::function;
using std::unordered_map;

extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, bool> is_coin;

extern int frame_count;

function<void(sprite_pair)> update_animation = [](const sprite_pair p) {
  entity_id id = p.first;
  sprite_component sprite = sprites[id];

  if (is_coin[id]) {
    if (frame_count % 10 == 0) {
      sprite.current_clip = (sprite.current_clip + 1) % sprite.num_clips;
      sprite.src.x = sprite.current_clip * sprite.src.w;
      sprites[id] = sprite;
    }
  } else if (sprite.is_animating) {
    sprite.current_clip = (sprite.current_clip + 1) % sprite.num_clips;
    sprite.src.x = sprite.current_clip * sprite.src.w;
    sprites[id] = sprite;
  }
};

void update_animations() {
  for_each(sprites.begin(), sprites.end(), update_animation);
}
