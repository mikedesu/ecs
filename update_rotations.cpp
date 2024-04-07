#include <algorithm>
#include <functional>
#include <unordered_map>

#include "entity_id.h"
#include "rotation_pair.h"
#include "transform_component.h"

using std::for_each;
using std::function;
using std::unordered_map;

extern unordered_map<entity_id, bool> is_rotating;
extern unordered_map<entity_id, transform_component> transforms;

function<void(rotation_pair)> handle_rotation = [](const rotation_pair p) {
  entity_id id = p.first;
  bool is_rotating = p.second;
  if (is_rotating) {
    transform_component transform = transforms[id];
    transform.angle += 2.0;
    transforms[id] = transform;
  }
};

void update_rotations() {
  for_each(is_rotating.begin(), is_rotating.end(), handle_rotation);
}
