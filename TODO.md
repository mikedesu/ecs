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
- [x] count number of knives fired

## 07 April 2024

### 02:08 CST

- [x] re-organized entire codebase into individual files for each function
- [ ] need to make an Asset Manager in order to
    - [ ] load textures, fonts, sounds, etc
        - [ ] verify the file locations exist on disk
        - [ ] be generic enough to handle key-value name-filepath pairs
            - [ ] ex: "texture-dir", "img"
            - [ ] ex: "skull-texture", "skull-sheet.png"
            - [ ] ex: "font-dir", "fonts"
            - [ ] ex: "gfont", "hack.ttf"
            - [ ] ex: "eyeball-texture", "eyeball-sheet.png"




