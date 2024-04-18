#pragma once

#include "SDL_handler.h"
#include "enemy_type.h"

typedef struct {
  enemy_type type;
  bool active;
  int group;
  int cooldown;
  int cooldown_reduction;
} generator_component;

typedef struct {
  double x;
  double y;
  double vx;
  double vy;
  double angle;
  double scale;
} transform_component;

typedef struct {
  bool is_animating;
  int current_clip;
  int num_clips;
  SDL_Texture *texture;
  SDL_Rect src;
  SDL_Rect dest;
} sprite_component;
