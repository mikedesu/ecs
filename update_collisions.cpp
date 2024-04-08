extern void update_knife_collisions();
extern void update_skull_collisions();

void update_collisions() {
  update_knife_collisions();
  update_skull_collisions();
}
