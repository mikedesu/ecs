
#include "SDL_handler.h"

extern SDL_Rect debug_texture_src;
extern SDL_Rect debug_texture_dest;
extern int mWidth;
extern int mHeight;

void init_debug_texture_rects() {
  debug_texture_src.x = debug_texture_src.y = debug_texture_dest.x =
      debug_texture_dest.y = 0;
  debug_texture_src.w = debug_texture_dest.w = mWidth / 8;
  debug_texture_src.h = debug_texture_dest.h = mHeight;
}
