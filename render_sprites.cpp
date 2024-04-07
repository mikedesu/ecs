#include <algorithm>
#include <functional>
#include <unordered_map>
// #include <pair>

#include "SDL_handler.h"
#include "entity_id.h"
#include "sprite_component.h"
#include "sprite_pair.h"
#include "transform_component.h"

using std::for_each;
using std::function;
using std::unordered_map;

extern SDL_Renderer *renderer;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, bool> is_flipped;

function<void(sprite_pair)> draw_sprite = [](const sprite_pair p) {
  entity_id id = p.first;
  SDL_RenderCopyEx(renderer, p.second.texture, &p.second.src, &p.second.dest,
                   transforms[id].angle, NULL,
                   is_flipped[id] ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
};

void render_sprites() { for_each(sprites.begin(), sprites.end(), draw_sprite); }
