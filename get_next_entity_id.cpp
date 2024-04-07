#include "entity_id.h"

extern entity_id next_entity_id;

entity_id get_next_entity_id() { return next_entity_id++; }
