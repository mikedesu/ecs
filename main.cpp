#include "SDL_handler.h"
#include "bg_entity_type.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "generator_component.h"
#include "powerup_type.h"
#include "sprite_component.h"
#include "transform_component.h"
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

using std::default_random_engine;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

int DEBUG_TEXT_WRAP_LEN = 2048;

double zoom = 1.0; // has to appear

const int default_window_width = 1600;
const int default_window_height = 960;
const int default_knife_speed = 4;
const int default_knife_cooldown = 30;

char texture_text[1024] = "a bunch of random text";
int target_texture_width = 1600;
int target_texture_height = 960;
int debug_font_size = 16;
int soulshard_spawn_rate = 25.0;

bool quit = false;
bool do_render_debug_panel = true;
bool is_fullscreen = false;
int window_width = default_window_width;
int window_height = default_window_height;
int knife_cooldown = 0;
int current_knife_cooldown = default_knife_cooldown;
int num_collisions = 0;
int current_knife_speed = default_knife_speed;
int img_flags = IMG_INIT_PNG;
int result = -1;
int w = 0;
int h = 0;
int frame_count = 0;
int mWidth = -1;
int mHeight = -1;

int num_knives = 2;
int max_num_knives = 2;
int knife_charge = 2;

int num_knives_fired = 0;
int num_enemies_escaped = 0;
int fullscreen_width = -1;
int fullscreen_height = -1;
int player_health = 3;
int player_max_health = 3;
int player_money = 0;

int color_index = 0;
int num_color_indices = 3;

string skullsheet_filepath = "img/skull-sheet4x.png";
// string skullsheet_filepath = "img/blackskull-sheet4x.png";
string eyeballsheet_filepath = "img/eyeball-sheet4x.png";
string soulshard_sheet_filepath = "img/soulshard-001-sheet4x.png";

entity_id next_entity_id = 0;
entity_id player_id = -1;

TTF_Font *gFont = nullptr;

SDL_Color textColor = {255, 255, 255, 255};
SDL_Event e;
SDL_Rect debug_texture_src;
SDL_Rect debug_texture_dest;
SDL_Rect target_texture_src;
SDL_Rect target_texture_dest;
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Surface *text_surface = nullptr;
SDL_Texture *target_texture = nullptr;
SDL_Texture *debug_texture = nullptr;
SDL_Texture *debug_bg_texture = nullptr;

vector<entity_id> entities;
vector<entity_id> entities_marked_for_deletion_tmp;

unordered_map<int, bool> is_pressed;
unordered_map<entity_id, powerup_type> powerup_types;
unordered_map<entity_id, enemy_type> enemy_types;

unordered_map<entity_id, sprite_component> sprites;
unordered_map<entity_id, transform_component> transforms;

unordered_map<entity_id, sprite_component> bg_sprites;
unordered_map<entity_id, transform_component> bg_transforms;
unordered_map<entity_id, bg_entity_type> bg_entity_types;

unordered_map<entity_id, double> rotation_speeds;

unordered_map<entity_id, generator_component> generators;
unordered_map<entity_id, bool> inputs;
unordered_map<entity_id, bool> is_rotating;
unordered_map<entity_id, bool> is_collidable;
unordered_map<entity_id, bool> is_enemy;
unordered_map<entity_id, bool> is_soulshard;
unordered_map<entity_id, bool> is_knife;
unordered_map<entity_id, bool> is_powerup;
unordered_map<entity_id, bool> is_flipped;
unordered_map<entity_id, bool> is_generator;
unordered_map<entity_id, bool> is_marked_for_deletion;
unordered_map<enemy_type, int> enemies_killed;
unordered_map<powerup_type, int> powerups_collected;
unordered_map<string, SDL_Texture *> textures;

// random number generator
default_random_engine rng_generator;
uniform_real_distribution<double> eyeball_vx_distribution;
uniform_real_distribution<double> soulshard_spawn_rate_distribution;
uniform_real_distribution<double> texture_height_distribution;

int init_target_texture();
entity_id get_next_entity_id();
void bg_init();
void cleanup();
void cleanup_entities_marked_for_deletion();
void create_window();
void create_renderer();
void handle_input();
void handle_input_component();
void handle_init_target_texture();
void init_debug_texture_rects();
void init_target_texture_rects();
void init();
void load_debug_text();
void load_textures();
void render_frame();
void spawn_skull();
void spawn_generator(enemy_type type, bool active, int cooldown);
void update();

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  create_window();
  create_renderer();
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  init();
  handle_init_target_texture();
  load_textures();
  load_debug_text();
  init_debug_texture_rects();
  // get the width and height of the texture
  init_target_texture_rects();
  bg_init();
  spawn_skull();

  // spawn_generator(ENEMY_TYPE_EYEBALL, true, 120);
  spawn_generator(ENEMY_TYPE_BAT, true, 128);

  while (!quit) {
    handle_input();
    handle_input_component();
    update();
    render_frame();
    // knife_cooldown = (knife_cooldown > 0) ? knife_cooldown - 1 : 0;
    knife_cooldown--;
    if (knife_cooldown <= 0) {
      knife_cooldown = 0;
    }

    // playing around with speeding up eyeball generation on a timer
    // every 30 seconds or 1800 frames, cut it in half
    // prob need a realistic cap on this...
    const int cooldown_min = 10;
    if (frame_count > 0 && frame_count % 3600 == 0) {
      for (auto id : entities) {
        if (is_generator[id]) {
          generators[id].cooldown = generators[id].cooldown / 2;
          if (generators[id].cooldown < cooldown_min) {
            generators[id].cooldown = cooldown_min;
          }
        }
      }
    }

    cleanup_entities_marked_for_deletion();
  }
  cleanup();
  return EXIT_SUCCESS;
}
