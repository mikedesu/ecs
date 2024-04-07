
#include "SDL_handler.h"

extern SDL_Rect target_texture_src;
extern SDL_Rect target_texture_dest;
extern int target_texture_width;
extern int target_texture_height;
extern int window_width;
extern int window_height;

void init_target_texture_rects() {
  target_texture_src.x = 0;
  target_texture_src.y = 0;
  target_texture_dest.x = 0;
  target_texture_dest.y = 0;
  target_texture_src.w = target_texture_width;
  target_texture_src.h = target_texture_height;
  target_texture_dest.w = window_width;
  target_texture_dest.h = window_height;
}
