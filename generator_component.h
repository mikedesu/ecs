#pragma once

#include "enemy_type.h"

typedef struct {
  enemy_type type;
  bool active;
  int cooldown;
} generator_component;
