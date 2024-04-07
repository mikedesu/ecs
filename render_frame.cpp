
#include "SDL_handler.h"

extern SDL_Renderer *renderer;
extern SDL_Texture *target_texture;
extern SDL_Rect target_texture_src;
extern SDL_Rect target_texture_dest;

extern bool do_render_debug_panel;
extern int frame_count;

extern void render_sprites();
extern void render_debug_panel();
extern void load_debug_text();

void render_frame() {
  // clear the target texture
  SDL_SetRenderTarget(renderer, target_texture);
  SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
  SDL_RenderClear(renderer);
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
