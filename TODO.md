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
    - [x] needs tuning, basically "uncatchable" after so-many speedups/sizeups
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
    - [x] knife speed increase
    - [x] knife size increase
    - [x] knife spread increase (shotgun)
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
- [x] restart game after game-over
- [x] pause game
- [x] un-pause game
- [x] powerup that increases movement speed
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
    - [x] magnet range
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
- [ ] powerup to make the player smaller
- [x] powerup to fire an additional knife
    - [x] when combined with the 'spray' powerup this will become powerful
    - [ ] spawning a lot of extra knives is appearing to have a big slowdown effect on the game
        - [ ] possibly due to the rectangle modulation
            - [ ] one way we might fix this is to pre-calculate the destination rectangle
- [x] put a cap on how many player-speed powerups you can get
    - [x] currently, moving too fast is possible after getting so-many speedups
- [x] converted sprites to a `map` but it isn't seeming to improve performance at all
    - [ ] attempting to cut blood pixel count in half seemed to work, but there is still a significant performance slowdown around ~2000 entities on screen
    - [ ] it is possible that attempting to manage blood pixel lifetimes is causing the slowdown...
        - [ ] because we are spawning blood pixels in groups, we could manage lifetimes with a group instead of per-pixel

## 28 April 2024

- [ ] gravity component (for blood pixels among other things)
    - [ ] would modify the transform to drag the entity down
- [ ] pumpkin enemy / boss
    - [ ] motivation: secret of mana boss 2
- [-] consider changing the x,y in `transform_component` to `int` from `double` 
- [-] consider removing ALL `double` computations
    - not happening!
- [ ] unlimit blood particle generation
- [ ] **BIG DEAL** move `transform_component` properties into `sprite_component`
    - [ ] all code locations that would use `transform.x` would now use `sprite.dest.x`
    - [ ] hoping this process gives us a performance boost
    - [ ] we want to see smooth 60fps at 2k entities
        - [ ] 2k
        - [ ] 3k
        - [ ] 4k
        - [ ] 5k
        - [ ] 10k
        - [ ] 20k
- [ ] consider pre-generating blood pixels
    - [ ] time to spawn 4k pixels is very high
    - [ ] time to handle transforms for 4k entities is very high

## 30 April 2024

- [ ] we want to be able to tune powerups such that
    - [ ] how many powerups are on-screen at any given point
        - [ ] during powerup-spawn, we need to check how many are currently not-picked up
    - [ ] we can specify which powerups the player starts with at the beginning
    - [ ] hard limits on how many of each powerup will matter 
        - [ ] this will allow us to better specify which powerups should spawn 
    - [ ] a map to specify which powerups are currently spawning or not
        - [ ] we will be able to turn powerup-spawns for specific powerups on/off 
        - [ ] this will still allow the random powerup selection to occur, but omit certain powerups

- [ ] spawn enemies going left to right
    - [ ] need to specify vx/vy in the generator
    - [ ] need to specify the spawn location

- [ ] spawn delay timer
    - [ ] need to specify a field for what frame an enemy begins spawning

- [ ] possibly make generators generic for any entity type  

## 1 May 2024

- [x] beginning of eyeball enemy testing
- [x] somehow i broke the candle animation, need to fix that
    - [x] it is rendering all 3 frames as if it is failing to parse numclips

## 3 May 2024

- [x] working on enemy damage flashes (FIXED)
    - [x] for some reason bat texture is fucking up
    - [x] tried hard-coding "bat-dmg" key on spawn and removed "bat" but game won't load...
    - [x] need to trace thru the loading and find where it is getting stuck


## 4 May 2024

- [ ] gameover screen is kind of boring
- [ ] we could begin implementing complex movements
    - [ ] lock onto player
    - [ ] sinewave movement
    - [ ] multi-phase movement
    - [ ] spawning from top or bottom of screen
- [ ] powerup tweaks
    - [x] limit the number of speed-ups
    - [ ] implement skull-shrink powerup
        - [ ] reduces the sprite dest rect by some amount
- [ ] spawn rate for eyeballs 
- [x] pass scale into enemy generators
- [x] pass HP into enemy generators
- [x] generator counters / some "way" to turn them on/off
- [ ] some "way" to remove generators / clean them up when they are off
- [ ] clean up `update_generators`

## 9 May 2024

- [ ] asset packing
    - [ ] do we NEED to do this?
        - [ ] too much work to encrypt/decrypt/pack assets in general
        - [ ] to what benefit? someone will just reverse-engineer everything anyway
        - [ ] the game is open source for a reason...
        - [ ] ideally, other people could make and use their own graphics if they wish!
- [ ] sound effects
    - [ ] need some free oldschool 8-bit-ish sound effects to just throw in for various in-game events
        - [ ] getting hit
        - [ ] picking up a powerup
        - [ ] moving
        - [ ] firing a knife
        - [ ] enemy death
        - [ ] enemy hit
        - [ ] gameover
        - [ ] pause
        - [ ] game restart

## 15 May 2024

- [x] goblin bullets
    - [x] fires directly up
    - [x] fires at the player
    - [ ] fires in a spread
- [ ] three classes of goblins
    - [x] one that fires bullets up
    - [ ] one that fires bullets at the player
    - [ ] one that fires bullets in a spread
- [ ] eyeballs currently only spawn in one y-position
    - [ ] need to randomize the y-position
- [ ] need to consider moving gameconfig out of a map and into its own `struct` again


## 16 May 2024

- [ ] need to make sure data structures are properly cleaned-up after gameover and restart
    - [x] goblins are spawning because the current frame is not being reset
        - [x] need to distinguish the total frame_count for the game the whole time it is being run versus the total frame_count for this play thru (i.e. after game over)
- [ ] powerup spawn positions are incorrect, can appear too-far above/below screen
- [ ] need to pass in the bullet parameters to a JSON config, either generators or something else
    - [ ] bullet frequency
    - [ ] bullet speed
    - [ ] count of bullets per round
    - [ ] cooldown time between rounds
    - [ ] bullet movement (directly up, left, down, right, spread, at player, homing)
- [ ] dynamic movement for enemies  

## 11 June 2024

- [ ] realized generators are currently geared towards enemies and doesnt include things like powerups or collidable boxes so we need to modify generators to be more generic so we can handle arbitary entities

