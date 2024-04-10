# TODO

## 03 April 2024

### 15:01 CDT

- [ ] Find a way to make the debug_texture into an entity 
    - [ ] May not be necessary?
- [ ] Find a way to bundle the textures and the rects into an unordered_map for easy management
    - [x] Textures done
    - [ ] Rects may not be necessary?

## 04 April 2024

### 13:56 CDT

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
    - [x] power-ups
- [x] scaling up graphics to make 4x the new 'default' 1x
- [x] faster movement using the SHIFT key
- [ ] knife "catching" mechanic
- [x] generic enemy generator
    - [x] eyeball generator
- [x] count number of knives fired

## 07 April 2024

### 02:08 CDT

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

### 19:28 CDT

- [ ] coins
    - [x] spawn on eyeball kill with 25% chance
    - [x] spawn on every bat kill 
- [ ] power-ups
    - [x] knife cooldown reduction
    - [ ] knife speed increase
    - [x] knife size increase
    - [ ] knife spread increase (shotgun)
    - [ ] knife angular rotation (damage)
- [ ] enemy spawn wave pressure
    - [x] increasing the generator rate
        - [ ] increasing the eyeball generation rate every N kills
        - [ ] increasing the eyeball generation rate every powerup activation
        - [ ] increasing the eyeball generation rate every N frames
    - [ ] spawning from both sides of the screen
        - [ ] do we need another generator data field?
- [ ] eyeball up/down bounce
- [ ] eyeballs bouncing off each other
- [x] there is a bug where eyeballs disappear when colliding with coins **fixed 08 April 2024 11:35 CST**

### 22:13 CDT

- [ ] re-factor generators to take a generic `entity_type` parameter
    - `ENTITY_TYPE_ENEMY_EYEBALL`
    - `ENTITY_TYPE_KNIFE`
    - `ENTITY_TYPE_COIN`
    - [-] may not be necessary

## 08 April 2024

### 16:21 CDT 

- [x] realizing we can probably combine functions from multiple files into individual function-group files
    - [x] ex: `spawn_skull`, `spawn_knife`, etc. can probably go in `spawn.c`
    - [x] this would achieve faster compile time with the same organizational benefit while also reducing LOC
- [x] group `load_texture` functions into their own `load.c` file

## 09 April 2024

### 18:21 CDT

- [ ] the game crashed several 10k+ frames in
    - [ ] spawn rate may have been too high
    - [ ] need to build some debug logging infrastructure for when this happens 
        - [ ] need to write out stats to a file
    - [x] appears to not be crashing, survived 100k frame renders
- [ ] we can do cool things with colors such as
    - [ ] load the skull texture once, then color it differently and save as a separate texture
        - [ ] this would allow us to hot-swap sprite textures on-the-fly

### 22:02 CDT

- [ ] popup texts for various messages
- [ ] handle getting hit by an enemy
    - [ ] colliding with enemy
        - [x] one-damage 
        - [ ] lose all powerups
- [ ] enemy death animation texture
    - [ ] eyeball
    - [ ] bat
- [ ] cap on size multiplier for knives
- [ ] powerup that increases the knife velocity
- [ ] game over screen

## 10 April 2024

### 11:24 CDT

- [x] knife-limit mechanic
- [ ] knife-catching mechanic
- [ ] game-over screen with debug stats
    - [ ] enemies killed
    - [ ] enemies escaped
    - [ ] damaged by # of enemies
- [ ] restart game after game-over
- [ ] pause game
- [ ] un-pause game
- [ ] powerup that increases movement speed
- [ ] negative-powerup that increases enemy movement speed
- [ ] negative-powerup that increases enemy spawn rate
- [ ] negative-powerup that increases enemy size
- [ ] negative-powerup that increases enemy health

## 15:35 CDT

- [x] knives should be able to collide with multiple enemies at the same time
    - [x] for each knife, check each other entity and mark all entities it collides with, as well as marking the knife itself


## 16:06 CDT

- [ ] powerup that decreases skull size
- [ ] enemy bullets
- [ ] bullet-grazing mechanic
- [ ] different-colored power-up textures to distinguish them
    - [ ] some texture besides the 'scroll'
- [ ] HUD
    - [ ] knife count
    - [ ] powerup count
    - [ ] score
    - [ ] health
    - [ ] time played
- [ ] background textures
    - [ ] moon
    - [ ] candles
    - [ ] lightning
    - [ ] clouds
    - [ ] gravestones
    - [ ] trees
    - [ ] parallax scrolling





