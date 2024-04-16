#include "SDL_handler.h"
#include "components.h"
#include "entity_id.h"
#include "gameconfig.h"
#include "mPrint.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include <SDL2/SDL_render.h>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

#define RAPIDJSON_NOMEMBERITERATORCLASS 1

using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::Value;

using std::snprintf;
using std::string;
using std::unordered_map;
using std::vector;

// external variables
extern gameconfig config;
extern int default_window_width;
extern int default_window_height;
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

// external functions
extern double fps();
extern size_t get_num_enemies_killed();
extern void cleanup_and_exit_with_failure();
extern void cleanup_and_exit_with_failure_mprint(string msg);

// declarations
void load_debug_text();
void load_texture(string key, string path);
void load_texture_with_color_mod(string key, string path, Uint8 r, Uint8 g,
                                 Uint8 b);
void load_pixel(string key, Uint8 r, Uint8 g, Uint8 b, int w, int h);
void handle_load_pixel(Value &v);
bool json_value_has_member_is_string(Value &v, string member);
void handle_load_texture_with_color_mod(Value &v);
void handle_load_texture(Value &v);
Document load_textures_document();
Document load_main_config_document();
void check_if_json_value_has_member_and_is_string(Value &v, string member);
void handle_load_by_type(Value &v);
void check_if_json_value_is_object(Value &v);
void load_textures();

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

void handle_load_pixel(Value &v) {
  string keys[] = {"key", "r", "g", "b", "w", "h"};
  for (string key : keys) {
    if (!v.HasMember(key.c_str())) {
      string msg = "config/textures.json array element has no " + key;
      mPrint(msg);
      cleanup_and_exit_with_failure();
    }
  }

  if (!v["r"].IsInt() || !v["g"].IsInt() || !v["b"].IsInt() ||
      !v["w"].IsInt() || !v["h"].IsInt()) {
    string msg = "config/textures.json array element has invalid r, g, b, w, "
                 "or h";
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }

  if (!v["key"].IsString()) {
    string msg = "config/textures.json array element has invalid key";
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }

  string key = v["key"].GetString();
  int r = v["r"].GetInt();
  int g = v["g"].GetInt();
  int b = v["b"].GetInt();
  int w = v["w"].GetInt();
  int h = v["h"].GetInt();
  load_pixel(key, r, g, b, w, h);
}

bool json_value_has_member_is_string(Value &v, string member) {
  return v.HasMember(member.c_str()) && v[member.c_str()].IsString();
}

void handle_load_texture_with_color_mod(Value &v) {
  string keys[] = {"key", "path", "r", "g", "b"};
  string integer_keys[] = {"r", "g", "b"};
  string str_keys[] = {"key", "path"};
  for (string key : keys) {
    if (!v.HasMember(key.c_str())) {
      string msg = "config/textures.json array element has no " + key;
      mPrint(msg);
      cleanup_and_exit_with_failure();
    }
  }

  for (string key : integer_keys) {
    if (!v[key.c_str()].IsInt()) {
      string msg = "config/textures.json array element has invalid " + key;
      mPrint(msg);
      cleanup_and_exit_with_failure();
    }
  }

  for (string key : str_keys) {
    if (!v[key.c_str()].IsString()) {
      string msg = "config/textures.json array element has invalid " + key;
      mPrint(msg);
      cleanup_and_exit_with_failure();
    }
  }

  string key = v["key"].GetString();
  string path = v["path"].GetString();
  int r = v["r"].GetInt();
  int g = v["g"].GetInt();
  int b = v["b"].GetInt();
  load_texture_with_color_mod(key, path, r, g, b);
}

void handle_load_texture(Value &v) {
  if (!json_value_has_member_is_string(v, "path")) {
    string msg = "config/textures.json array element has no path";
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }
  string path = v["path"].GetString();
  string key = v["key"].GetString();
  load_texture(key, path);
}

Document load_textures_document() {
  string config_file_path = "config/textures.json";
  const size_t read_buffer_size = 65536;
  char readBuffer[read_buffer_size];
  Document d;
  FILE *fp = fopen(config_file_path.c_str(), "r");
  if (fp == nullptr) {
    mPrint("Failed to open " + config_file_path);
    cleanup_and_exit_with_failure();
  }
  fread(readBuffer, 1, read_buffer_size, fp);
  fclose(fp);
  d.Parse(readBuffer);
  if (d.HasParseError()) {
    string msg = "Failed to parse config/textures.json";
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }
  return d;
}

void check_if_json_value_has_member_and_is_string(Value &v, string member) {
  if (!v.HasMember(member.c_str()) || !v[member.c_str()].IsString()) {
    string msg = "config/textures.json array element has no " + member;
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }
}

void check_if_json_value_is_object(Value &v) {
  if (!v.IsObject()) {
    string msg = "config/textures.json array element is not an object";
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }
}

void handle_load_by_type(Value &v) {
  string type = v["type"].GetString();
  if (type == "texture") {
    handle_load_texture(v);
  } else if (type == "texture_colormod") {
    handle_load_texture_with_color_mod(v);
  } else if (type == "pixel") {
    handle_load_pixel(v);
  } else {
    string msg = "config/textures.json array element has unknown type: " + type;
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }
}

void load_textures() {
  Document d = load_textures_document();
  if (d.IsArray()) {
    for (auto &v : d.GetArray()) {
      check_if_json_value_is_object(v);
      check_if_json_value_has_member_and_is_string(v, "type");
      check_if_json_value_has_member_and_is_string(v, "key");
      handle_load_by_type(v);
    }
  }
}

Document load_main_config_document() {
  string config_file_path = "config/game.json";
  const size_t read_buffer_size = 65536;
  char readBuffer[read_buffer_size];
  Document d;
  FILE *fp = fopen(config_file_path.c_str(), "r");
  if (fp == nullptr) {
    mPrint("Failed to open " + config_file_path);
    cleanup_and_exit_with_failure();
  }
  fread(readBuffer, 1, read_buffer_size, fp);
  fclose(fp);
  d.Parse(readBuffer);
  if (d.HasParseError()) {
    string msg = "Failed to parse " + config_file_path;
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }
  return d;
}

void load_main_config() {
  Document d = load_main_config_document();
  if (d.HasMember("debug_font_size") && d["debug_font_size"].IsInt()) {
    config.debug_font_size = d["debug_font_size"].GetInt();
  } else {
    mPrint("config/game.json has no debug_font_size");
    mPrint("setting to a default");
    config.debug_font_size = 16;
  }

  if (d.HasMember("default_window_width") &&
      d["default_window_width"].IsInt()) {
    default_window_width = d["default_window_width"].GetInt();
  } else {
    mPrint("config/game.json has no default_window_width");
    mPrint("setting to a default");
    default_window_width = 640;
  }
  //
  if (d.HasMember("default_window_height") &&
      d["default_window_height"].IsInt()) {
    default_window_height = d["default_window_height"].GetInt();
  } else {
    mPrint("config/game.json has no default_window_height");
    mPrint("setting to a default");
    default_window_height = 480;
  }

  window_width = default_window_width;
  window_height = default_window_height;

  target_texture_width = default_window_width;
  target_texture_height = default_window_height;
}
