#pragma once

#include "SDL_handler.h"
#include "enemy_type.h"

typedef enum {
  SCREEN_POSITION_CENTER,
  SCREEN_POSITION_TOP,
  SCREEN_POSITION_BOTTOM,
  SCREEN_POSITION_LEFT,
  SCREEN_POSITION_RIGHT,
  SCREEN_POSITION_TOP_LEFT,
  SCREEN_POSITION_TOP_RIGHT,
  SCREEN_POSITION_BOTTOM_LEFT,
  SCREEN_POSITION_BOTTOM_RIGHT,
  SCREEN_POSITION_COUNT
} screen_position_t;

typedef struct {
  enemy_type type;
  bool active;
  int group;
  int cooldown;
  int cooldown_reduction;
  int frame_begin;
  int spawn_count;
  int hp;
  screen_position_t screen_position;
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
  SDL_Texture *dmg_texture;
  int dmg_frames;
  SDL_Rect src;
  SDL_Rect dest;
} sprite_component;
