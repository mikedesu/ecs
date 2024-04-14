#include "SDL_handler.h"
// #include "enemy_type.h"
#include "components.h"
#include "entity_id.h"
#include "mPrint.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <SDL2/SDL_render.h>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::Writer;

using std::snprintf;
using std::string;
using std::unordered_map;
using std::vector;

extern int target_texture_width;
extern int target_texture_height;
extern int window_width;
extern int window_height;
extern int frame_count;
extern int num_collisions;
extern int knife_cooldown;
extern int knife_charge;
extern int current_knife_cooldown;
extern int num_knives_fired;
extern int num_knives;
extern int max_num_knives;
extern int num_enemies_escaped;
extern int player_health;
extern int player_max_health;
extern int player_soulshards;
extern int total_soulshards_collected;
extern int w, h;
extern double zoom;
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
extern unordered_map<entity_id, transform_component> transforms;

extern entity_id player_id;
extern double fps();
extern size_t get_num_enemies_killed();
extern void cleanup_and_exit_with_failure_mprint(string msg);

void load_debug_text() {
  snprintf(texture_text, 1024,
           "target texture: %dx%d\nwindow size: %dx%d\nframe_count: "
           "%06d\nnum_entities: %ld\n"
           "fps: %.02f\nzoom: %.02f\nnum_collisions: "
           "%d\nknife_cooldown: %d\ncurrent_knife_cooldown: "
           "%d\nnum_knives :%d\nmax_num_knives: %d\nnum_knives_fired: "
           "%d\nknife_charge: %d\nnum_enemies_escaped: "
           "%d\nnum_enemies_killed: %ld\nplayer_health: "
           "%d/%d\nsoulshards_collected: %d\ntotal_soulshards_collected: "
           "%d\nplayer position: %.02f,%.02f\n",
           target_texture_width, target_texture_height, window_width,
           window_height, frame_count, entities.size(), fps(), zoom,
           num_collisions, knife_cooldown, current_knife_cooldown, num_knives,
           max_num_knives, num_knives_fired, knife_charge, num_enemies_escaped,
           get_num_enemies_killed(), player_health, player_max_health,
           player_soulshards, total_soulshards_collected,
           transforms[player_id].x, transforms[player_id].y);
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

void load_texture(string key, string path) {
  SDL_Texture *t = IMG_LoadTexture(renderer, path.c_str());
  if (t == nullptr) {
    string msg =
        "Failed to load texture image with key and path: " + key + "," + path;
    cleanup_and_exit_with_failure_mprint(msg);
  }
  textures[key] = t;
}

void load_texture_with_color_mod(string key, string path, Uint8 r, Uint8 g,
                                 Uint8 b) {
  SDL_Texture *t = IMG_LoadTexture(renderer, path.c_str());
  if (t == nullptr) {
    string msg =
        "Failed to load texture image with key and path: " + key + "," + path;
    cleanup_and_exit_with_failure_mprint(msg);
  }
  SDL_SetTextureColorMod(t, r, g, b);
  textures[key] = t;
}

void load_pixel(string key, Uint8 r, Uint8 g, Uint8 b, int w, int h) {
  SDL_Texture *pixel = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_TARGET, w, h);
  if (pixel == nullptr) {
    string msg = "Failed to create pixel texture with key: " + key;
    cleanup_and_exit_with_failure_mprint(msg);
  }
  SDL_SetRenderTarget(renderer, pixel);
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderTarget(renderer, nullptr);
  textures[key] = pixel;
}

void load_textures() {
  load_texture("skull", "img/skull-sheet4x.png");
  load_texture("eyeball", "img/eyeball-sheet4x.png");
  load_texture("knife", "img/knife-alt4x.png");
  load_texture("soulshard", "img/soulshard-white-sheet4x.png");
  // load_texture_with_color_mod("soulshard", "img/soulshard-sheet4x.png");
  load_texture("powerup", "img/powerup-sheet4x.png");
  load_texture("bat", "img/bat-sheet4x.png");
  load_texture("moon", "img/moon-0a4x.png");

  load_texture_with_color_mod("knife-blue", "img/knife-alt4x.png", 0, 0, 255);

  load_pixel("blood-pixel", 255, 0, 0, 4, 4);

  FILE *fp = fopen("config/textures.json", "r");
  if (fp == nullptr) {
    string msg = "Failed to open config/textures.json";
    cleanup_and_exit_with_failure_mprint(msg);
  }

  char readBuffer[65536];
  fread(readBuffer, 1, 65536, fp);
  fclose(fp);

  Document d;
  d.Parse(readBuffer);

  if (d.HasParseError()) {
    string msg = "Failed to parse config/textures.json";
    cleanup_and_exit_with_failure_mprint(msg);
  }

  if (!d.IsObject()) {
    string msg = "config/textures.json is not an object";
    cleanup_and_exit_with_failure_mprint(msg);
  }

  cout << d["test"].GetString() << endl;
}
