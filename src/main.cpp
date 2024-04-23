#include "SDL_handler.h"
#include "bg_entity_type.h"
#include "components.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "entity_type.h"
#include "mPrint.h"
#include "powerup_type.h"
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

// moving into gameconfig soon
double zoom = 1.0; // has to appear

// moving into gameconfig soon
const int default_knife_speed = 4;
int current_knife_speed = default_knife_speed;

int current_soulshard_magnetism_threshold = 100;

// moving into gameconfig soon
const int default_knife_cooldown = 30;
int knife_cooldown = 0;
int current_knife_cooldown = default_knife_cooldown;

char texture_text[1024] = "a bunch of random text";

bool quit = false;
bool do_render_debug_panel = false;
bool is_fullscreen = false;
bool is_paused = false;
bool is_gameover = false;

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

// int color_index = 0;
// int num_color_indices = 3;

entity_id next_entity_id = 0;
entity_id player_id = -1;

TTF_Font *gFont = nullptr;
TTF_Font *gameover_font = nullptr;

SDL_Color textColor = {255, 255, 255, 255};
SDL_Event e;
SDL_Rect debug_texture_src;
SDL_Rect debug_texture_dest;
SDL_Rect target_texture_src;
SDL_Rect target_texture_dest;
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Surface *text_surface = nullptr;
SDL_Surface *gameover_surface = nullptr;
SDL_Texture *target_texture = nullptr;
SDL_Texture *debug_texture = nullptr;
SDL_Texture *gameover_texture = nullptr;
SDL_Texture *debug_bg_texture = nullptr;

vector<entity_id> entities;
vector<entity_id> entities_marked_for_deletion_tmp;
vector<double> bat_vx_vec;
vector<int> bat_y_vec;

unordered_map<entity_id, powerup_type> powerup_types;
unordered_map<entity_id, enemy_type> enemy_types;
unordered_map<entity_id, sprite_component> sprites;
unordered_map<entity_id, transform_component> transforms;
unordered_map<entity_id, transform_component> bg_transforms;
unordered_map<entity_id, bg_entity_type> bg_entity_types;
unordered_map<entity_id, double> rotation_speeds;
unordered_map<entity_id, generator_component> generators;
unordered_map<entity_id, entity_type> entity_types;

unordered_map<int, bool> is_pressed;
unordered_map<entity_id, bool> inputs;
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

unordered_map<entity_id, int> blood_pixel_lifetime;
unordered_map<enemy_type, int> enemies_killed;
unordered_map<powerup_type, int> powerups_collected;
unordered_map<string, SDL_Texture *> textures;

map<entity_id, sprite_component> bg_sprites;

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
// void cleanup_data_structures();
void create_window();
void create_renderer();
void handle_input();
void handle_input_component();
void handle_init_target_texture();
void init_debug_texture_rects();
void init_target_texture_rects();
void init();
void init_after_load_textures();
void load_debug_text();
void load_textures();
void load_main_config();
void render_frame();
void render_gameover();
void spawn_skull(const int x, const int y);
void spawn_generator(enemy_type type, bool active, int group, int cooldown,
                     int cooldown_reduction);
void update();
void init_game();

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  load_main_config();
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

  init_game();
  // bg_init();
  // init_after_load_textures();
  // spawn_skull(0, 0);
  // spawn_generator(ENEMY_TYPE_BAT, true, 2, 240, 60 * 60);

  mPrint("main loop...");
  // mPrint("player_id: " + to_string(player_id));
  while (!quit) {
    handle_input();
    if (!is_paused && !is_gameover) {
      handle_input_component();
      update();
      render_frame();
      cleanup_entities_marked_for_deletion();
    } else if (is_gameover) {
      // mPrint("gameover");
      render_gameover();
    }
  }
  cleanup();
  return EXIT_SUCCESS;
}
