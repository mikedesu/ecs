#include "SDL_handler.h"
#include "components.h"
#include "entity_id.h"
#include "sprite_pair.h"
#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>

using std::for_each;
using std::function;
using std::map;
using std::string;
using std::unordered_map;

extern int w;
extern int h;
extern int current_frame_count;
extern int total_frame_count;
extern int window_width, window_height;
extern unordered_map<string, size_t> config;
extern SDL_Renderer *renderer;
extern map<entity_id, sprite_component> sprites;
extern map<entity_id, sprite_component> bg_sprites;
extern unordered_map<entity_id, transform_component> transforms;
extern unordered_map<entity_id, transform_component> bg_transforms;
extern unordered_map<entity_id, bool> is_flipped;
extern SDL_Texture *debug_texture;
extern SDL_Texture *gameover_texture;
extern SDL_Texture *stopwatch_texture;
extern SDL_Rect debug_texture_src;
extern SDL_Rect stopwatch_texture_src;
extern SDL_Rect stopwatch_texture_dest;
extern SDL_Rect debug_texture_dest;
extern SDL_Rect target_texture_src;
extern SDL_Rect target_texture_dest;
extern bool do_render_debug_panel;
extern SDL_Texture *target_texture;

extern void load_debug_text();
extern void load_stopwatch_text();

function<void(sprite_pair)> draw_sprite = [](const sprite_pair p) {
  const entity_id id = p.first;
  SDL_Texture *t = p.second.texture;
  if (p.second.dmg_frames > 0) {
    t = sprites[id].dmg_texture;
    sprites[id].dmg_frames--;
  }
  // SDL_RenderCopyEx(renderer, p.second.texture, &p.second.src, &p.second.dest,
  SDL_RenderCopyEx(renderer, t, &p.second.src, &p.second.dest,
                   transforms[id].angle, NULL,
                   is_flipped[id] ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
};

inline void render_debug_panel() {
  SDL_Color color = {0, 0, 0, 128};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &debug_texture_dest);
  SDL_RenderCopy(renderer, debug_texture, &debug_texture_src,
                 &debug_texture_dest);
}

inline void render_stopwatch() {
  SDL_RenderCopy(renderer, stopwatch_texture, &stopwatch_texture_src,
                 &stopwatch_texture_dest);
}

inline void render_sprites() {
  for_each(sprites.begin(), sprites.end(), draw_sprite);
}

inline void render_bg_sprites() {
  for_each(bg_sprites.rbegin(), bg_sprites.rend(), draw_sprite);
}

void render_gameover() {
  SDL_SetRenderTarget(renderer, target_texture);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_QueryTexture(gameover_texture, NULL, NULL, &w, &h);
  const int x = window_width / 2 - w / 2;
  const int y = window_height / 2 - h / 2;
  SDL_Rect src = {0, 0, w, h};
  SDL_Rect dest = {x, y, w, h};
  SDL_RenderCopy(renderer, gameover_texture, &src, &dest);
  SDL_SetRenderTarget(renderer, NULL);
  SDL_RenderCopy(renderer, target_texture, &target_texture_src,
                 &target_texture_dest);
  SDL_RenderPresent(renderer);
  total_frame_count++;
  current_frame_count++;
}

void render_frame() {
  SDL_SetRenderTarget(renderer, target_texture);
  SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
  SDL_RenderClear(renderer);
  render_bg_sprites();
  render_sprites();
  SDL_SetRenderTarget(renderer, NULL);
  SDL_RenderCopy(renderer, target_texture, &target_texture_src,
                 &target_texture_dest);
  render_stopwatch();
  load_stopwatch_text();
  if (do_render_debug_panel) {
    render_debug_panel();
    load_debug_text();
  }
  SDL_RenderPresent(renderer);
  current_frame_count++;
  total_frame_count++;
}
