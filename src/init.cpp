#include "SDL_handler.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "mPrint.h"
#include "powerup_type.h"
#include <SDL_render.h>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

using std::mt19937;
using std::random_device;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

extern random_device rd;
extern mt19937 g;

extern unordered_map<string, SDL_Texture *> textures;
extern unordered_map<string, size_t> config;
extern unordered_map<powerup_type, int> powerups_collected;

extern vector<double> bat_vx_vec;
extern vector<int> bat_y_vec;

extern SDL_Rect debug_texture_src;
extern SDL_Rect debug_texture_dest;
extern SDL_Rect target_texture_src;
extern SDL_Rect target_texture_dest;
extern SDL_Rect player_hud_texture_src;
extern SDL_Rect player_hud_texture_dest;
extern SDL_Rect stopwatch_texture_src;
extern SDL_Rect stopwatch_texture_dest;

extern bool is_gameover;

extern entity_id player_id;

extern int powerups_onscreen;
extern int cooldown_min;
extern int mWidth;
extern int mHeight;
extern int current_knife_speed;
extern int default_knife_speed;
extern int default_player_speed;
extern int current_player_speed;
extern int current_soulshard_magnetism_threshold;
extern int num_knives;
extern int max_num_knives;
extern int knife_charge;
extern int num_knives_fired;
extern int num_enemies_escaped;
extern int player_health;
extern int player_max_health;
extern int player_soulshards;
extern int total_soulshards_collected;
extern int debug_font_size;
extern int img_flags;
extern int result;

extern int knife_cooldown;
extern int current_knife_cooldown;
extern int default_knife_cooldown;

extern unsigned long game_begin_time;

extern TTF_Font *gFont;
extern TTF_Font *gameover_font;

extern SDL_Renderer *renderer;
extern SDL_Texture *target_texture;

extern uniform_real_distribution<double> unit_distribution;
extern uniform_real_distribution<double> eyeball_vx_distribution;
extern uniform_real_distribution<double> soulshard_spawn_rate_distribution;
extern uniform_real_distribution<double> texture_height_distribution;
extern uniform_real_distribution<double> blood_velocity_positive_distribution;
extern uniform_real_distribution<double> blood_velocity_negative_distribution;
extern uniform_real_distribution<double> blood_velocity_distribution;

extern void cleanup_and_exit_with_failure();
extern void cleanup_and_exit_with_failure_mprint(string message);
extern void cleanup_data_structures();
extern void bg_init();
extern void spawn_skull(const int x, const int y);
extern void spawn_generator(enemy_type type, bool active, int group,
                            int cooldown, int cooldown_reduction);

void init_game_vars();
void init_bat_vectors();
void init_after_load_textures();
void init_debug_texture_rects();
void init_fonts();
void init_img();
void init_rng();
void init_ttf();
void handle_init_target_texture();
void init_target_texture_rects();
void init();
int init_target_texture();

void init_stopwatch_texture_rects() {
  stopwatch_texture_src.x = stopwatch_texture_src.y = stopwatch_texture_dest.x =
      stopwatch_texture_dest.y = 0;
  stopwatch_texture_src.w = stopwatch_texture_dest.w = mWidth;
  stopwatch_texture_src.h = stopwatch_texture_dest.h = mHeight;
}

void init_player_hud_texture_rects() {
  player_hud_texture_src.x = player_hud_texture_src.y =
      player_hud_texture_dest.x = player_hud_texture_dest.y = 0;
  player_hud_texture_src.w = player_hud_texture_dest.w = mWidth / 4;
  player_hud_texture_src.h = player_hud_texture_dest.h = mHeight;
}

void init_debug_texture_rects() {
  debug_texture_src.x = debug_texture_src.y = debug_texture_dest.x =
      debug_texture_dest.y = 0;
  debug_texture_src.w = debug_texture_dest.w = mWidth / 4;
  debug_texture_src.h = debug_texture_dest.h = mHeight;
}

void init_fonts() {
  string path = "ttf/hack.ttf";
  gFont = TTF_OpenFont(path.c_str(), config["debug_font_size"]);
  if (gFont == nullptr) {
    mPrint("Failed to load font: " + path);
    cleanup_and_exit_with_failure();
  }

  const int gameover_fontsize = 128;
  gameover_font = TTF_OpenFont(path.c_str(), gameover_fontsize);
  if (gameover_font == nullptr) {
    mPrint("Failed to load font: " + path);
    cleanup_and_exit_with_failure();
  }
}

void init_img() {
  result = IMG_Init(img_flags);
  if ((result & img_flags) != img_flags) {
    cleanup_and_exit_with_failure_mprint(
        "IMG_Init: Failed to init required png support: " +
        string(IMG_GetError()));
  }
}

void init_rng() {
  srand(time(nullptr));
  g = mt19937(rd());
  eyeball_vx_distribution = uniform_real_distribution<double>(-4.0, -2.0);
  soulshard_spawn_rate_distribution =
      uniform_real_distribution<double>(0.0, 100.0);
  if (config["target_texture_height"] > 0) {
    texture_height_distribution =
        uniform_real_distribution<double>(0.0, config["target_texture_height"]);
  }
  blood_velocity_positive_distribution =
      uniform_real_distribution<double>(0.0, 2.0);
  blood_velocity_negative_distribution =
      uniform_real_distribution<double>(-2.0, 0.0);
  blood_velocity_distribution = uniform_real_distribution<double>(-2.0, 2.0);

  unit_distribution = uniform_real_distribution<double>(-1.0, 1.0);
}

int init_target_texture() {
  if (config.find("target_texture_width") == config.end() ||
      config.find("target_texture_height") == config.end()) {
    mPrint("Target texture width or height not found in config!");
    return 0;
  }
  if (config["target_texture_width"] == 0 ||
      config["target_texture_height"] == 0) {
    mPrint("Target texture width or height is 0!");
    return 0;
  }
  target_texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      config["target_texture_width"], config["target_texture_height"]);
  if (target_texture == nullptr) {
    mPrint("Failed to create target texture!");
    return 0;
  }
  return 1;
}

void handle_init_target_texture() {
  result = init_target_texture();
  if (!result) {
    mPrint("Failed to init target texture");
    cleanup_and_exit_with_failure();
  }
}

void init_target_texture_rects() {
  target_texture_src.x = 0;
  target_texture_src.y = 0;
  target_texture_dest.x = 0;
  target_texture_dest.y = 0;
  target_texture_src.w = config["target_texture_width"];
  target_texture_src.h = config["target_texture_height"];
  target_texture_dest.w = config["window_width"];
  target_texture_dest.h = config["window_height"];
}

void init_ttf() {
  result = TTF_Init();
  if (result == -1) {
    mPrint("Failed to init TTF" + string(TTF_GetError()));
    cleanup_and_exit_with_failure();
  }
}

void init() {
  init_img();
  init_ttf();
  init_fonts();
  init_rng();
}

void init_bat_vectors() {
  bat_vx_vec.push_back(-4.0);
  bat_vx_vec.push_back(-3.5);
  bat_vx_vec.push_back(-3.0);
  bat_vx_vec.push_back(-2.5);
  bat_vx_vec.push_back(-2.0);
  bat_vx_vec.push_back(-1.5);

  SDL_Texture *t = textures["bat"];
  int w, h;
  SDL_QueryTexture(t, NULL, NULL, &w, &h);

  size_t y = 0;
  while (y < config["target_texture_height"] - h) {
    bat_y_vec.push_back(y);
    y += h * 2;
  }
}

void init_after_load_textures() {
  init_bat_vectors();
  init_game_vars();
}

void init_game_vars() {
  player_id = -1;
  game_begin_time = SDL_GetTicks();

  // a lot of these can be loaded from game.json
  num_knives = 2;
  max_num_knives = 2;
  knife_charge = 2;
  num_knives_fired = 0;
  num_enemies_escaped = 0;
  player_health = 3;
  player_max_health = 3;
  player_soulshards = 0;
  total_soulshards_collected = 0;
  current_knife_speed = default_knife_speed;
  knife_cooldown = 0;
  current_knife_cooldown = default_knife_cooldown;
  is_gameover = false;
  current_soulshard_magnetism_threshold = config["default_magnetism_threshold"];

  default_player_speed = 8;
  current_player_speed = default_player_speed;

  // cooldown_min = 1; // LOL
  // cooldown_min = 10; // hard
  cooldown_min = 30; // do-able no lag

  powerups_collected.clear();

  powerups_onscreen = 0;
}

void init_game() {
  cleanup_data_structures();
  bg_init();
  init_after_load_textures();
  spawn_skull(0, 0);
  // spawn_generator(ENEMY_TYPE_BAT, true, 2, 60 * 30, 60 * 30);
  spawn_generator(ENEMY_TYPE_BAT, true, 2, 60 * 4, 60 * 30);
  // spawn_generator(ENEMY_TYPE_BAT, true, 2, 1, 60 * 30);
}
