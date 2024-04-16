
#include "SDL_handler.h"
#include "gameconfig.h"
#include "mPrint.h"
#include <random>
#include <string>

using std::string;
using std::uniform_real_distribution;

extern gameconfig config;
extern SDL_Rect debug_texture_src;
extern SDL_Rect debug_texture_dest;
extern SDL_Rect target_texture_src;
extern SDL_Rect target_texture_dest;
extern int mWidth;
extern int mHeight;
extern int soulshard_spawn_rate;
extern int debug_font_size;
extern int img_flags;
extern int result;
extern int target_texture_width;
extern int target_texture_height;
extern int window_width;
extern int window_height;
extern TTF_Font *gFont;
extern SDL_Renderer *renderer;
extern SDL_Texture *target_texture;
extern uniform_real_distribution<double> eyeball_vx_distribution;
extern uniform_real_distribution<double> soulshard_spawn_rate_distribution;
extern uniform_real_distribution<double> texture_height_distribution;
extern uniform_real_distribution<double> blood_velocity_positive_distribution;
extern uniform_real_distribution<double> blood_velocity_negative_distribution;
extern uniform_real_distribution<double> blood_velocity_distribution;

extern void cleanup_and_exit_with_failure();
extern void cleanup_and_exit_with_failure_mprint(string message);

void init_debug_texture_rects() {
  debug_texture_src.x = debug_texture_src.y = debug_texture_dest.x =
      debug_texture_dest.y = 0;
  debug_texture_src.w = debug_texture_dest.w = mWidth / 4;
  debug_texture_src.h = debug_texture_dest.h = mHeight;
}

void init_gfont() {
  // gFont = TTF_OpenFont("ttf/hack.ttf", debug_font_size);
  string path = "ttf/hack.ttf";
  gFont = TTF_OpenFont(path.c_str(), config.debug_font_size);
  if (gFont == nullptr) {
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
  eyeball_vx_distribution = uniform_real_distribution<double>(-4.0, -2.0);
  soulshard_spawn_rate_distribution =
      uniform_real_distribution<double>(0.0, 100.0);

  if (target_texture_width > 0) {
    texture_height_distribution =
        uniform_real_distribution<double>(0.0, target_texture_height);
  }

  blood_velocity_positive_distribution =
      uniform_real_distribution<double>(1.0, 2.0);

  blood_velocity_negative_distribution =
      uniform_real_distribution<double>(-2.0, -1.0);

  blood_velocity_distribution = uniform_real_distribution<double>(-2.0, 2.0);
}

int init_target_texture() {
  target_texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      target_texture_width, target_texture_height);
  if (target_texture == nullptr) {
    mPrint("Failed to create target texture!");
    return 0;
  }
  return 1;
}

void handle_init_target_texture() {
  result = init_target_texture();
  if (!result) {
    cleanup_and_exit_with_failure_mprint("Failed to init target texture");
  }
}

void init_target_texture_rects() {
  target_texture_src.x = 0;
  target_texture_src.y = 0;
  target_texture_dest.x = 0;
  target_texture_dest.y = 0;
  target_texture_src.w = target_texture_width;
  target_texture_src.h = target_texture_height;
  target_texture_dest.w = window_width;
  target_texture_dest.h = window_height;
}

void init_ttf() {
  result = TTF_Init();
  if (result == -1) {
    cleanup_and_exit_with_failure_mprint("Failed to init TTF" +
                                         string(TTF_GetError()));
  }
}

void init() {
  init_img();
  init_ttf();
  init_gfont();
  init_rng();
}
