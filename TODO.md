# TODO

## 03 April 2024

### 15:01 CST

- [ ] Find a way to make the debug_texture into an entity 
    - [ ] May not be necessary?
- [ ] Find a way to bundle the textures and the rects into an unordered_map for easy management
    - [x] Textures done
    - [ ] Rects may not be necessary?

## 04 April 2024

### 13:56 CST

- [ ] knives
    - [x] spawning
    - [x] movement
    - [x] removal when off-screen
    - [ ] collision with player
    - [x] collision with enemies
- [ ] distinguishing entity types
    - [ ] player
        - [x] capturing the player's `entity_id` on `spawn_skull()`
    - [x] enemies
    - [x] knives
    - [ ] power-ups
- [x] scaling up graphics to make 4x the new 'default' 1x
- [x] faster movement using the SHIFT key
- [ ] knife "catching" mechanic
- [x] generic enemy generator
    - [x] eyeball generator
- [ ] count number of knives fired
