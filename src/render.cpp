#include "SDL_handler.h"
#include "components.h"
#include "entity_id.h"
#include "sprite_pair.h"
#include <algorithm>
#include <functional>
#include <unordered_map>

using std::for_each;
using std::function;
using std::unordered_map;

extern SDL_Renderer *renderer;
extern unordered_map<entity_id, sprite_component> sprites;
extern unordered_map<entity_id, sprite_component> bg_sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, transform_component> bg_transforms;
extern unordered_map<entity_id, bool> is_flipped;
extern SDL_Texture *debug_texture;
extern SDL_Rect debug_texture_src;
extern SDL_Rect debug_texture_dest;
extern SDL_Rect target_texture_src;
extern SDL_Rect target_texture_dest;
extern bool do_render_debug_panel;
extern int frame_count;
extern SDL_Texture *target_texture;

extern void load_debug_text();

function<void(sprite_pair)> draw_sprite = [](const sprite_pair p) {
  const entity_id id = p.first;
  SDL_RenderCopyEx(renderer, p.second.texture, &p.second.src, &p.second.dest,
                   transforms[id].angle, NULL,
                   is_flipped[id] ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
};

void render_debug_panel() {
  const SDL_Color color = {0, 0, 0, 128};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &debug_texture_dest);
  SDL_RenderCopy(renderer, debug_texture, &debug_texture_src,
                 &debug_texture_dest);
}

void render_sprites() { for_each(sprites.begin(), sprites.end(), draw_sprite); }
void render_bg_sprites() {
  for_each(bg_sprites.begin(), bg_sprites.end(), draw_sprite);
}

void render_frame() {
  // clear the target texture
  SDL_SetRenderTarget(renderer, target_texture);
  SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
  SDL_RenderClear(renderer);
  render_bg_sprites();
  render_sprites();
  //  reset the render target
  SDL_SetRenderTarget(renderer, NULL);
  SDL_RenderCopy(renderer, target_texture, &target_texture_src,
                 &target_texture_dest);
  // render debug text
  if (do_render_debug_panel) {
    render_debug_panel();
    load_debug_text();
  }
  SDL_RenderPresent(renderer);
  frame_count++;
}
