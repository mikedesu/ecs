#include "enemy_type.h"
#include "entity_id.h"
#include "generator_component.h"
#include <unordered_map>

using std::unordered_map;

extern int frame_count;
extern unordered_map<entity_id, generator_component> generators;
extern void spawn_eyeball();

void update_generators() {
  for (auto kv : generators) {
    entity_id id = kv.first;
    generator_component generator = kv.second;
    if (generator.active && frame_count % generator.cooldown == 0) {
      switch (generator.type) {
      case ENEMY_TYPE_EYEBALL:
        spawn_eyeball();
        break;
      default:
        break;
      }
    }
  }
}
