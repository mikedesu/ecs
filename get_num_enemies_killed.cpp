
#include "enemy_type.h"
#include <unordered_map>

using std::unordered_map;

extern unordered_map<enemy_type, int> enemies_killed;

size_t get_num_enemies_killed() {
  size_t total = 0;
  for (auto kv : enemies_killed) {
    total += kv.second;
  }
  return total;
}
