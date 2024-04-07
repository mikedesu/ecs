#include "SDL_handler.h"

extern SDL_Renderer *renderer;
extern SDL_Texture *debug_texture;
extern SDL_Rect debug_texture_src;
extern SDL_Rect debug_texture_dest;

void render_debug_panel() {
  SDL_Color color = {0, 0, 0, 128};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &debug_texture_dest);
  SDL_RenderCopy(renderer, debug_texture, &debug_texture_src,
                 &debug_texture_dest);
}
