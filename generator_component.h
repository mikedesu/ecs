#pragma once

#include "enemy_type.h"

typedef struct {
  enemy_type type;
  int timer;
  int cooldown;
} generator_component;
