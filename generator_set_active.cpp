// #include "enemy_type.h"
#include "entity_id.h"
#include "generator_component.h"
#include "mPrint.h"
#include <unordered_map>
#include <vector>

using std::unordered_map;
using std::vector;

extern int frame_count;
extern unordered_map<entity_id, generator_component> generators;
extern unordered_map<entity_id, bool> is_generator;
extern vector<entity_id> entities;
// extern void spawn_eyeball();

void generator_set_active(entity_id id, bool active) {
  if (generators.find(id) != generators.end()) {
    generator_component generator = generators[id];
    generator.active = active;
    generators[id] = generator;
  }
}

void generator_set_active_flip(entity_id id) {
  mPrint("generator_set_active_flip");
  if (is_generator[id]) {
    generator_component generator = generators[id];
    generator.active = !generator.active;
    generators[id] = generator;
  }
}

void generator_set_all_active_flip() {
  for (auto id : entities) {
    generator_set_active_flip(id);
  }
}

void generator_set_all_active() {
  for (auto id : entities) {
    generator_set_active(id, true);
  }
}

void generator_set_all_inactive() {
  for (auto id : entities) {
    generator_set_active(id, false);
  }
}
