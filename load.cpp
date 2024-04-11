#include "SDL_handler.h"
// #include "enemy_type.h"
#include "entity_id.h"
#include "mPrint.h"
#include <SDL2/SDL_render.h>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

using std::snprintf;
using std::string;
using std::unordered_map;
using std::vector;

extern string skullsheet_filepath;
extern int target_texture_width;
extern int target_texture_height;
extern int window_width;
extern int window_height;
extern int frame_count;
extern int num_collisions;
extern int knife_cooldown;
extern int current_knife_cooldown;
extern int num_knives_fired;
extern int num_knives;
extern int max_num_knives;
extern int num_enemies_escaped;
extern int player_health;
extern int player_max_health;
extern int player_money;
extern int w, h;
extern double zoom;
extern string coin_sheet_filepath;
extern char texture_text[1024];
extern SDL_Texture *debug_texture;
extern SDL_Surface *text_surface;
extern SDL_Color textColor;
extern TTF_Font *gFont;
extern SDL_Renderer *renderer;
extern int mWidth;
extern int mHeight;
extern int DEBUG_TEXT_WRAP_LEN;
extern vector<entity_id> entities;
extern unordered_map<string, SDL_Texture *> textures;
extern string eyeballsheet_filepath;

extern double fps();
extern size_t get_num_enemies_killed();

void load_debug_text() {
  snprintf(texture_text, 1024,
           "target texture: %dx%d\nwindow size: %dx%d\nframe_count: "
           "%06d\nnum_entities: %ld\n"
           "fps: %.02f\nzoom: %.02f\nnum_collisions: "
           "%d\nknife_cooldown: %d\ncurrent_knife_cooldown: "
           "%d\nnum_knives :%d\nmax_num_knives: %d\nnum_knives_fired: "
           "%d\nnum_enemies_escaped: "
           "%d\nnum_enemies_killed: %ld\nplayer_health: "
           "%d/%d\ncoins_collected: %d\n",
           target_texture_width, target_texture_height, window_width,
           window_height, frame_count, entities.size(), fps(), zoom,
           num_collisions, knife_cooldown, current_knife_cooldown, num_knives,
           max_num_knives, num_knives_fired, num_enemies_escaped,
           get_num_enemies_killed(), player_health, player_max_health,
           player_money);
  text_surface = TTF_RenderText_Blended_Wrapped(gFont, texture_text, textColor,
                                                DEBUG_TEXT_WRAP_LEN);
  if (text_surface == nullptr) {
    mPrint("text_surface == nullptr");
    // printf("textureText = %s\n", textureText);
    printf("Unable to render text_surface! SDL_ttf Error: %s\n",
           TTF_GetError());
  } else {
    // Create texture from surface pixels
    if (debug_texture != nullptr) {
      SDL_DestroyTexture(debug_texture);
      debug_texture = nullptr;
    }
    debug_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (debug_texture == nullptr) {
      mPrint("mTexture == NULL");
      printf("Unable to create texture from rendered text! SDL Error: %s\n",
             SDL_GetError());
    }
    // Get image dimensions
    mWidth = text_surface->w;
    mHeight = text_surface->h;
    // Get rid of old surface
    SDL_FreeSurface(text_surface);
    text_surface = nullptr;
  }
}

extern void cleanup_and_exit_with_failure_mprint(string msg);

void load_coin_sheet_texture() {
  SDL_Texture *t = IMG_LoadTexture(renderer, coin_sheet_filepath.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         coin_sheet_filepath);
  }
  textures["coin"] = t;
}

void load_eyeball_sheet_texture() {
  SDL_Texture *t = IMG_LoadTexture(renderer, eyeballsheet_filepath.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         eyeballsheet_filepath);
  }
  textures["eyeball"] = t;
}

void load_bat_sheet_texture() {
  string filepath = "img/bat-sheet4x.png";
  SDL_Texture *t = IMG_LoadTexture(renderer, filepath.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         filepath);
  }
  textures["bat"] = t;
}

void load_skull_sheet_texture() {

  SDL_Surface *s = IMG_Load(skullsheet_filepath.c_str());
  if (s == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load image: " +
                                         skullsheet_filepath);
  }

  SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
  // SDL_Texture *t = IMG_LoadTexture(renderer, skullsheet_filepath.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         skullsheet_filepath);
  }

  SDL_Texture *tmp_t[3] = {nullptr, nullptr, nullptr};
  for (int i = 0; i < 3; i++) {
    tmp_t[i] = SDL_CreateTextureFromSurface(renderer, s);
    if (tmp_t[i] == nullptr) {
      cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                           skullsheet_filepath);
    }
  }

  SDL_SetTextureColorMod(tmp_t[0], 255, 0, 0);
  SDL_SetTextureColorMod(tmp_t[1], 0, 255, 0);
  SDL_SetTextureColorMod(tmp_t[2], 0, 0, 255);

  textures["skull"] = t;
  textures["skull-red"] = tmp_t[0];
  textures["skull-green"] = tmp_t[1];
  textures["skull-blue"] = tmp_t[2];
  SDL_FreeSurface(s);
}

void load_knife_sheet_texture() {
  // string filepath = "img/knife4x.png";
  string filepath = "img/knife-alt4x.png";
  SDL_Texture *t = IMG_LoadTexture(renderer, filepath.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         filepath);
  }
  // SDL_SetTextureColorMod(t, 255, 0, 0);
  textures["knife"] = t;
}

void load_powerup_sheet_texture() {
  string path = "img/powerup-sheet4x.png";
  SDL_Texture *t = IMG_LoadTexture(renderer, path.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         path);
  }
  textures["powerup"] = t;
}

void load_moon_texture() {
  string path = "img/moon-0a4x.png";
  SDL_Texture *t = IMG_LoadTexture(renderer, path.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         path);
  }
  textures["moon"] = t;
}

void load_textures() {
  load_skull_sheet_texture();
  load_eyeball_sheet_texture();
  load_knife_sheet_texture();
  load_coin_sheet_texture();
  load_powerup_sheet_texture();
  load_bat_sheet_texture();
  load_moon_texture();
}
