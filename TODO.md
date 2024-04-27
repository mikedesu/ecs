# TODO

## 03 April 2024

### 15:01 CDT

- [ ] Find a way to make the debug_texture into an entity 
    - [ ] May not be necessary?
- [x] Find a way to bundle the textures and the rects into an unordered_map for easy management
    - [x] Textures done
    - [x] Rects may not be necessary?
        - [x] this is handled via transform

## 04 April 2024

### 13:56 CDT

- [x] knives
    - [x] spawning
    - [x] movement
    - [x] removal when off-screen
    - [x] collision with player
    - [x] collision with enemies
- [ ] distinguishing entity types
    - [ ] player
        - [x] capturing the player's `entity_id` on `spawn_skull()`
    - [x] enemies
    - [x] knives
    - [x] power-ups
- [x] scaling up graphics to make 4x the new 'default' 1x
- [x] faster movement using the SHIFT key
- [x] knife "catching" mechanic
    - [ ] needs tuning, basically "uncatchable" after so-many speedups/sizeups
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
            - [x] ex: "skull-texture", "skull-sheet.png"
            - [ ] ex: "font-dir", "fonts"
            - [ ] ex: "gfont", "hack.ttf"
            - [x] ex: "eyeball-texture", "eyeball-sheet.png"

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
        - [x] increasing the eyeball generation rate every N frames
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

- [x] the game crashed several 10k+ frames in
    - [-] spawn rate may have been too high
    - [-] need to build some debug logging infrastructure for when this happens 
        - [-] need to write out stats to a file
    - [x] appears to not be crashing, survived 100k frame renders
- [x] we can do cool things with colors such as
    - [x] load the skull texture once, then color it differently and save as a separate texture
        - [x] this would allow us to hot-swap sprite textures on-the-fly

### 22:02 CDT

- [ ] popup texts for various messages
- [ ] handle getting hit by an enemy
    - [x] colliding with enemy
        - [x] one-damage 
        - [ ] lose all powerups
- [ ] enemy death animation texture
    - [ ] eyeball
    - [ ] bat
        - [x] blood-pixel spawning
- [ ] cap on size multiplier for knives
- [x] powerup that increases the knife velocity
- [ ] game over screen

## 10 April 2024

### 11:24 CDT

- [x] knife-limit mechanic
- [x] knife-catching mechanic
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

### 15:35 CDT

- [x] knives should be able to collide with multiple enemies at the same time
    - [x] for each knife, check each other entity and mark all entities it collides with, as well as marking the knife itself


### 16:06 CDT

- [ ] powerup that decreases skull size
- [ ] enemy bullets
- [ ] enemy-grazing mechanic
- [ ] bullet-grazing mechanic
- [x] different-colored power-up textures to distinguish them
    - [ ] some texture besides the 'scroll'
- [ ] HUD / debug-panel
    - [x] knife count
    - [ ] powerup count
    - [ ] score
    - [x] health
    - [ ] time played this game
    - [ ] enemies grazed
    - [ ] bullets grazed
- [x] background textures
    - [x] one way to handle this might be to store them in a separate map and treat them separately from foreground entities
    - [x] moon
    - [ ] candles
    - [ ] lightning
    - [ ] clouds
    - [ ] gravestones
    - [ ] trees
    - [ ] parallax scrolling

## 12 April 2024

### 00:53 CDT

- [x] bug involving firing knives to the left
- [x] tweak knife cooldown default and minimum values
- [ ] moon bg texture loop around
- [x] re-name coins to 'soulshards' in the code

### 12:54 CDT

- [x] when you move close enough to a soulshard, it should magnetically attract to you

### 17:18 CDT

- [ ] enemy death animation
- [x] blood particle generator
- [ ] fire particle generator


## 13 April 2024

### 12:42 CDT

- [x] idea for blood splatter effect: use a transparent texture and randomly draw red pixels on it 
    - [ ] pre-render several different textures and randomize them on enemy death
    - [ ] fade them out over time then remove them from the scene
- [ ] explosion animation
- [ ] fire animation
- [ ] move the magnetic-soulshard variables into main so that they can be tune-able over time
    - [ ] magnet range
    - [ ] magnet speed
- [ ] gameover
    - [x] on player death, need to trigger some gameover event / state
    - [ ] need to reset the game state
        - [ ] will require a function
        - [ ] a lot of the functionality needed to do this exists now, but we need to organize it into a single function
        - [x] this way, on either game load or gameover, we can start a new game

### 23:08 CDT

- [x] fix the knife-throwing bug when the skull is facing left
    - [x] current implementation hard-codes smallest knife size
        - [x] when the knife-size scale increases, the resulting destination rects do 
        - [x] need to get the size of the scaled knife width in order to place it appropriately
        - [x] same with height, but the bug involves the width and x-position

### 23:52 CDT

- [x] bats currently 'pop' into existence due to the x,y position chosen
    - [x] this was initially to avoid them being removed from the scene early
    - [x] to fix this, extend the range for scene removal such that

```
removalboundary | spawnboundary | mainscreen | spawnboundary | removalboundary

spawnboundary is from 0 to -sprite.width and screen.width to screen.width + sprite.width
removalboundary is from -sprite.width to -2*sprite.width and screen.width + sprite.width to screen.width + 2*sprite.width
```

## 15 April 2024

### 13:02 CDT

- [x] we could spawn bats in groups of N
- [x] preparing for powerup texture color mods via JSON config file
- [x] loading textures from config/textures.json
    - [x] RapidJSON parsing
- [x] blood pixels defined in the texture.json file are not displaying at their configured width and height
    - [x] fixed, am now passing in defined width and height on pixel load
        - [ ] probably need to create a global configuration object that can be tuned at runtime

### 18:02 CDT

- [ ] thinking of detaching the bottom of the skull mouth so that it is a separate sprite
    - [ ] could lead to cool effects like long mouth opening for firing a laser or smth

## 16 April 2024

### 00:36 CDT

- [x] starting to do config via json files
    - [x] textures
    - [x] game config
        - [x] default window width and height
        - [x] debug panel font size
    - [x] config is now an unordered_map of string key and size_t value
- [x] magnetic pick-up of ALL items, not just soulshards

### 11:10 CDT

- [x] configurable number of blood particles per kill set in game.json
    - [ ] probably shouldn't allow this number to be set too high :X 
    - [x] fixed - re-wrote the knife-collision code to do explicit checks to make sure we aren't colliding with a blood pixel or other non-enemy

## 17 April 2024

### 23:19 CDT

- [ ] define sprites and num_clips in a sprites.json 
- [ ] magnetic pick-up as a global variable that can be tuned with powerups
- [ ] a powerup for magnetic pick-up range extension
- [ ] blood particle lifetime for removal before they reach the screen edge
    - [ ] this might be computationally-expensive but a worthwhile experiment
- [ ] powerups inspired off of ouija boards, tarot cards, etc

## 19 April 2024

### 00:11 CDT

- [ ] vampire bats could have teeth :E 

### 16:19 CDT

- [ ] when loading in textures from the JSON, we need to make sure we dont add an entry for a key that we have already defined, it might get overwritten otherwise

## 21 April 2024

### 16:40 CDT

- [x] when spawning groups of bats, we want to configure them such that
    - [x] each bat has a different vx
    - [x] each bat is spawned an equal distance away from each other
    - [x] each bat is spawned on-screen and able to be hit by the player

## 22 April 2024

### 13:34 CDT

- [ ] gameover text is not properly centered on fullscreen

### 15:34 CDT

- [x] all of the various in-game variables need initialization-functions because they are currently hardcoded in at the start and need to be reset on gameover-continue
    - [ ] will also be useful for saving state later on for implementing save game


### 23:13 CDT

- [x] implement a 'screenshot' key function
    - [x] `void screenshot()`
    - [x] hook into `handle_keydown()`

## 23 April 2024

### 23:02 CDT

- [ ] pass vx and vy into `spawn_generator()`
    - [ ] attach a vx and vy to a `generator`
    - [ ] when the `generator` needs to `spawn` entities or groups, we can use the `vx` and `vy` attributes we write

## 24 April 2024

- [ ] gamepad/controller support
    - [x] direction hats
    - [x] buttons
    - [ ] joysticks
        - [ ] discovered we cannot init joystick and read from keyboard
            - [ ] there has to be a workaround

## 25 April 2024

- [ ] there is a bug in FPS calculation since frame-draws stop when paused
    - [ ] prob want to just turn off world-state updates
    - [ ] arbitrary menu display / pause-menu indication

## 26 April 2024

- [x] spray powerup
- [ ] a way to disable the spawning of individual powerups
    - [ ] can have a vector of powerup types

## 27 April 2024

- [x] can start game in fullscreen 
    - [ ] need to add to config/game.json

