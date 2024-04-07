#include "entity_id.h"
#include "generator_component.h"

#include <unordered_map>
#include <vector>

using std::unordered_map;
using std::vector;

extern unordered_map<entity_id, generator_component> generators;
extern entity_id get_next_entity_id();
extern vector<entity_id> entities;

void spawn_generator(enemy_type type, int timer, int cooldown) {
  if (type > ENEMY_TYPE_COUNT) {
    // mPrint("invalid enemy type");
    return;
  }
  if (cooldown < 1) {
    // mPrint("invalid cooldown");
    return;
  }
  for (auto kv : generators) {
    generator_component generator = kv.second;
    if (generator.type == type) {
      // mPrint("generator for type " + std::to_string(type) + " already
      // exists");
      return;
    }
  }
  entity_id id = get_next_entity_id();
  generators[id] = {type, timer, cooldown};
  entities.push_back(id);
}
