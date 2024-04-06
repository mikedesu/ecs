#pragma once

#include "SDL_handler.h"

typedef struct {
  bool is_animating;
  int current_clip;
  int num_clips;
  SDL_Texture *texture;
  SDL_Rect src;
  SDL_Rect dest;
} sprite_component;
