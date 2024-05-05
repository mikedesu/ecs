#include "SDL_handler.h"
#include "bg_entity_type.h"
#include "components.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "entity_type.h"
#include "mPrint.h"
#include "powerup_type.h"
#include <SDL.h>
#include <SDL_joystick.h>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

using std::default_random_engine;
using std::map;
using std::mt19937;
using std::random_device;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

random_device rd;
mt19937 g;

unordered_map<string, size_t> config;

int DEBUG_TEXT_WRAP_LEN = 2048;

double zoom = 1.0; // has to appear

char texture_text[1024] = "texture_text";
char stopwatch_text[128] = "stopwatch_text";

bool quit = false;
bool do_render_debug_panel = false;
bool is_fullscreen = true;
bool is_paused = false;
bool is_gameover = false;

int cooldown_min = -1;
int window_width = -1;
int window_height = -1;
int default_knife_speed = 4;
int current_knife_speed = default_knife_speed;
int current_soulshard_magnetism_threshold = 100;
int default_knife_cooldown = 30;
int knife_cooldown = 0;
int current_knife_cooldown = default_knife_cooldown;
int default_player_speed = 8;
int current_player_speed = default_player_speed;
int num_collisions = 0;
int img_flags = IMG_INIT_PNG;
int result = -1;
int w = 0;
int h = 0;
int frame_count = 0;
int mWidth = -1;
int mHeight = -1;
int fullscreen_width = -1;
int fullscreen_height = -1;
int gameover_count = 0;
int num_knives = 2;
int max_num_knives = 2;
int knife_charge = 2;
int num_knives_fired = 0;
int num_enemies_escaped = 0;
int player_health = 3;
int player_max_health = 3;
int player_soulshards = 0;
int total_soulshards_collected = 0;
int powerups_onscreen = 0;

entity_id next_entity_id = 0;
entity_id player_id = -1;

unsigned long game_begin_time = 0;

TTF_Font *gFont = nullptr;
TTF_Font *stopwatch_font = nullptr;
TTF_Font *gameover_font = nullptr;

SDL_Color textColor = {255, 255, 255, 255};
SDL_Color debug_text_color = {255, 255, 255, 255};
SDL_Color stopwatch_text_color = {255, 0, 0, 255};
SDL_Event e;

SDL_Rect target_texture_src;
SDL_Rect target_texture_dest;
SDL_Rect debug_texture_src;
SDL_Rect debug_texture_dest;
SDL_Rect player_hud_texture_src;
SDL_Rect player_hud_texture_dest;
SDL_Rect stopwatch_texture_src;
SDL_Rect stopwatch_texture_dest;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Joystick *joystick = nullptr;
SDL_Surface *text_surface = nullptr;
SDL_Surface *stopwatch_surface = nullptr;
SDL_Surface *gameover_surface = nullptr;
SDL_Texture *target_texture = nullptr;
SDL_Texture *debug_texture = nullptr;
SDL_Texture *player_hud_texture = nullptr;
SDL_Texture *stopwatch_texture = nullptr;
SDL_Texture *gameover_texture = nullptr;
SDL_Texture *debug_bg_texture = nullptr;

vector<entity_id> entities;
vector<entity_id> entities_marked_for_deletion_tmp;
vector<double> bat_vx_vec;
vector<int> bat_y_vec;

map<entity_id, sprite_component> sprites;
map<entity_id, sprite_component> bg_sprites;
unordered_map<entity_id, int> hitpoints;
unordered_map<entity_id, powerup_type> powerup_types;
unordered_map<entity_id, enemy_type> enemy_types;
unordered_map<entity_id, transform_component> transforms;
unordered_map<entity_id, transform_component> bg_transforms;
unordered_map<entity_id, bg_entity_type> bg_entity_types;
unordered_map<entity_id, double> rotation_speeds;
unordered_map<entity_id, generator_component> generators;
unordered_map<entity_id, entity_type> entity_types;
unordered_map<entity_id, bool> inputs;
// unordered_map<entity_id, bool> is_damaged;
unordered_map<entity_id, bool> is_rotating;
unordered_map<entity_id, bool> is_collidable;
unordered_map<entity_id, bool> is_blood_pixel;
unordered_map<entity_id, bool> is_enemy;
unordered_map<entity_id, bool> is_soulshard;
unordered_map<entity_id, bool> is_knife;
unordered_map<entity_id, bool> is_powerup;
unordered_map<entity_id, bool> is_flipped;
unordered_map<entity_id, bool> is_generator;
unordered_map<entity_id, bool> is_marked_for_deletion;
unordered_map<entity_id, int> knife_charges;
unordered_map<entity_id, int> blood_pixel_lifetime;
unordered_map<entity_id, int> explosion_frames;
unordered_map<enemy_type, int> enemies_killed;
unordered_map<powerup_type, int> powerups_collected;
unordered_map<string, SDL_Texture *> textures;
unordered_map<string, int> num_clips;
unordered_map<int, bool> is_pressed;

// random number generator
default_random_engine rng_generator;
uniform_real_distribution<double> unit_distribution;
uniform_real_distribution<double> eyeball_vx_distribution;
uniform_real_distribution<double> soulshard_spawn_rate_distribution;
uniform_real_distribution<double> texture_height_distribution;
uniform_real_distribution<double> blood_velocity_positive_distribution;
uniform_real_distribution<double> blood_velocity_negative_distribution;
uniform_real_distribution<double> blood_velocity_distribution;

void cleanup();
void cleanup_entities_marked_for_deletion();
void create_window();
void create_renderer();
void handle_input();
void handle_input_component();
void handle_init_target_texture();
void init_debug_texture_rects();
void init_player_hud_texture_rects();
void init_target_texture_rects();
void init();
void load_debug_text();
void load_stopwatch_text();
void load_textures();
void load_main_config();
void render_frame();
void render_gameover();
void update();
void init_game();
void do_fullscreen();

int main() {
  load_main_config();
  SDL_Init(SDL_INIT_VIDEO);
  if (SDL_WasInit(SDL_INIT_VIDEO)) {
    mPrint("SDL Video initialized");
  } else {
    mPrint("failed to init SDL Video");
  }
  create_window();
  create_renderer();
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  init();
  handle_init_target_texture();
  load_textures();
  load_debug_text();
  init_debug_texture_rects();
  load_stopwatch_text();
  init_player_hud_texture_rects();
  init_target_texture_rects();
  do_fullscreen();
  init_game();
  while (!quit) {
    handle_input();
    if (!is_paused && !is_gameover) {
      handle_input_component();
      update();
      render_frame();
      cleanup_entities_marked_for_deletion();
    } else if (is_gameover) {
      render_gameover(); // prob should be handled in render_frame()
    }
  }
  cleanup();
  return EXIT_SUCCESS;
}
