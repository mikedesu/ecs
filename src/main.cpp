#include "SDL_handler.h"
#include "bg_entity_type.h"
#include "components.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "gameconfig.h"
#include "mPrint.h"
#include "powerup_type.h"
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

using std::default_random_engine;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

gameconfig config;

int DEBUG_TEXT_WRAP_LEN = 2048;

double zoom = 1.0; // has to appear

int default_window_width;
int default_window_height;
// int default_window_width = 1600;
// int default_window_height = 960;
int window_width;
int window_height;
// int window_width = default_window_width;
// int window_height = default_window_height;
const int default_knife_speed = 4;
const int default_knife_cooldown = 30;

char texture_text[1024] = "a bunch of random text";
int target_texture_width;
int target_texture_height;
// int target_texture_width = 1600;
// int target_texture_height = 960;
//   int debug_font_size = 16;
//    int soulshard_spawn_rate = 25.0;

bool quit = false;
bool do_render_debug_panel = true;
bool is_fullscreen = false;
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
int player_soulshards = 0;
int total_soulshards_collected = 0;

int color_index = 0;
int num_color_indices = 3;

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
unordered_map<entity_id, bool> is_blood_pixel;

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
uniform_real_distribution<double> blood_velocity_positive_distribution;
uniform_real_distribution<double> blood_velocity_negative_distribution;
uniform_real_distribution<double> blood_velocity_distribution;

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
void load_main_config();
void render_frame();
void spawn_skull();
void spawn_generator(enemy_type type, bool active, int cooldown,
                     int cooldown_reduction);
void update();

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  load_main_config();
  create_window();
  create_renderer();
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  mPrint("init...");
  init();
  mPrint("handle init...");
  handle_init_target_texture();
  mPrint("load textures...");
  load_textures();
  mPrint("load debug text...");
  load_debug_text();
  mPrint("init debug texture rects...");
  init_debug_texture_rects();
  // get the width and height of the texture
  mPrint("init target texture rects...");
  init_target_texture_rects();
  mPrint("bg init...");
  bg_init();
  mPrint("spawn skull...");
  spawn_skull();
  mPrint("spawn generator...");
  // spawn_generator(ENEMY_TYPE_EYEBALL, true, 120);
  spawn_generator(ENEMY_TYPE_BAT, true, 120, 60 * 60);
  mPrint("main loop...");

  while (!quit) {
    handle_input();
    handle_input_component();
    update();
    render_frame();
    cleanup_entities_marked_for_deletion();
  }
  cleanup();
  return EXIT_SUCCESS;
}
