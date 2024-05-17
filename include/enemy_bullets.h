#pragma once

// #include "enemy_type.h"

typedef enum {
  ENEMY_BULLET_MOVEMENT_UP,
  ENEMY_BULLET_MOVEMENT_TOWARDS_PLAYER,
} enemy_bullet_movement;

typedef struct {
  enemy_bullet_movement movement;
  int speed;
  int frequency;
  int cooldown;
  double scale;
} enemy_bullet_definition;
