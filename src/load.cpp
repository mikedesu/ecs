#include "SDL_handler.h"
#include "components.h"
#include "enemy_bullets.h"
#include "entity_id.h"
#include "mPrint.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include <SDL2/SDL_render.h>
#include <SDL_timer.h>
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

extern SDL_Color stopwatch_text_color;
extern int powerups_onscreen;
extern unordered_map<string, size_t> config;
extern unordered_map<enemy_type, enemy_bullet_definition>
    enemy_bullet_definitions;
extern int current_soulshard_magnetism_threshold;
// extern int frame_count;
extern int total_frame_count;
extern int current_frame_count;
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
extern char stopwatch_text[128];
extern SDL_Texture *debug_texture;
extern SDL_Texture *gameover_texture;
extern SDL_Texture *stopwatch_texture;
extern SDL_Surface *text_surface;
extern SDL_Surface *gameover_surface;
extern SDL_Surface *stopwatch_surface;
extern SDL_Color textColor;
extern SDL_Rect stopwatch_texture_src;
extern SDL_Rect stopwatch_texture_dest;
extern SDL_Renderer *renderer;
extern TTF_Font *gFont;
extern TTF_Font *stopwatch_font;
extern TTF_Font *gameover_font;
extern int mWidth;
extern int mHeight;
extern int DEBUG_TEXT_WRAP_LEN;
extern vector<entity_id> entities;
extern unordered_map<string, SDL_Texture *> textures;
extern unordered_map<entity_id, transform_component> transforms;
extern entity_id player_id;
extern int gameover_count;
extern unsigned long game_begin_time;
extern unordered_map<string, int> num_clips;
extern int window_width;
extern int window_height;

// inline double fps() { return frame_count / (SDL_GetTicks64() / 1000.0f); }
inline double fps() { return total_frame_count / (SDL_GetTicks64() / 1000.0f); }

// external functions
extern size_t get_num_enemies_killed();
extern void cleanup_and_exit_with_failure();
extern void cleanup_and_exit_with_failure_mprint(string msg);
extern void spawn_generator(enemy_type type, bool active, int group,
                            int cooldown, int cooldown_reduction,
                            int frame_begin, int spawn_count, int hp,
                            double scale, screen_position_t screen_position);

// declarations
void load_debug_text();
void load_texture(string key, string path, const int numclips);
void load_texture_with_color_mod(string key, string path, const int numclips,
                                 Uint8 r, Uint8 g, Uint8 b);
void load_pixel(string key, Uint8 r, Uint8 g, Uint8 b, int w, int h);
void handle_load_pixel(Value &v);
bool json_value_has_member_is_string(Value &v, string member);
void handle_load_texture_with_color_mod(Value &v);
void handle_load_texture(Value &v);
const Document load_document(string path);
void check_if_json_value_has_member_and_is_string(Value &v, string member);
void handle_load_texture_by_type(Value &v);
void check_if_json_value_is_object(Value &v);
void load_textures();
bool check_if_json_has_member_and_is_double(Value &v, string member);

bool check_if_json_has_member_and_is_int(Value &v, string member) {
  return v.HasMember(member.c_str()) && v[member.c_str()].IsInt();
}

bool check_if_json_has_member_and_is_double(Value &v, string member) {
  return v.HasMember(member.c_str()) && v[member.c_str()].IsDouble();
}

void load_gameover_texture() {
  // mPrint("load gameover texture");
  const string gameover_text = "GAMEOVER";
  gameover_surface = TTF_RenderText_Blended_Wrapped(
      gameover_font, gameover_text.c_str(), textColor, DEBUG_TEXT_WRAP_LEN);
  if (!gameover_surface) {
    mPrint("text_surface == nullptr");
    mPrint("Unable to render text_surface! SDL_ttf Error: " +
           string(TTF_GetError()));
  } else {
    // Create texture from surface pixels
    if (gameover_texture != nullptr) {
      SDL_DestroyTexture(debug_texture);
      gameover_texture = nullptr;
    }
    gameover_texture = SDL_CreateTextureFromSurface(renderer, gameover_surface);
    if (gameover_texture == nullptr) {
      mPrint("gameover_texture == NULL");
      mPrint("Unable to create texture from rendered text! SDL Error: " +
             string(SDL_GetError()));
    }
    SDL_FreeSurface(gameover_surface);
  }
}

void load_stopwatch_text() {
  // unsigned long milliseconds = SDL_GetTicks64() - game_begin_time;
  unsigned long milliseconds = SDL_GetTicks64() - game_begin_time;
  // SDL_GetTicks64() - game_begin_time + (99 * 60 * 1000);
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  seconds = seconds % 60;
  // snprintf(stopwatch_text, 128, "%lu", SDL_GetTicks64());
  snprintf(stopwatch_text, 128, "%lu:%02lu", minutes, seconds);
  stopwatch_surface =
      TTF_RenderText_Blended_Wrapped(stopwatch_font, stopwatch_text,
                                     stopwatch_text_color, DEBUG_TEXT_WRAP_LEN);
  // stopwatch_font, stopwatch_text, textColor, DEBUG_TEXT_WRAP_LEN);
  // gFont, stopwatch_text, textColor, DEBUG_TEXT_WRAP_LEN);
  if (stopwatch_surface == nullptr) {
    mPrint("stopwatch_surface == nullptr");
    mPrint("Unable to render stopwatch_surface! SDL_ttf Error: " +
           string(TTF_GetError()));
  } else {
    // Create texture from surface pixels
    if (stopwatch_texture != nullptr) {
      SDL_DestroyTexture(stopwatch_texture);
      stopwatch_texture = nullptr;
    }
    stopwatch_texture =
        SDL_CreateTextureFromSurface(renderer, stopwatch_surface);
    if (stopwatch_texture == nullptr) {
      mPrint("mTexture == NULL");
      mPrint("Unable to create texture from rendered text! SDL Error: " +
             string(SDL_GetError()));
    }
    // Get image dimensions
    mWidth = stopwatch_surface->w;
    mHeight = stopwatch_surface->h;
    // init_stopwatch_texture_rects();
    stopwatch_texture_src.x = 0;
    stopwatch_texture_src.y = 0;
    stopwatch_texture_dest.x = 0;
    stopwatch_texture_dest.y = 0;
    stopwatch_texture_src.w = stopwatch_texture_dest.w = stopwatch_surface->w;
    stopwatch_texture_src.h = stopwatch_texture_dest.h = stopwatch_surface->h;
    // Get rid of old surface
    SDL_FreeSurface(stopwatch_surface);
    stopwatch_surface = nullptr;
  }
}

void load_debug_text() {
  snprintf(texture_text, 1024,
           "target texture: %ldx%ld\nwindow size: "
           "%ldx%ld\ncurrent_frame_count: %06d\ntotal_frame_count: "
           "%06d\nnum_entities: %ld\n"
           "fps: %.02f\nzoom: %.02f\nnum_collisions: "
           "%d\nknife_cooldown: %d\ncurrent_knife_cooldown: "
           "%d\nnum_knives :%d\nmax_num_knives: %d\nnum_knives_fired: "
           "%d\nknife_charge: %d\nnum_enemies_escaped: "
           "%d\nnum_enemies_killed: %ld\nplayer_health: "
           "%d/%d\nsoulshards_collected: %d\ntotal_soulshards_collected: "
           "%d\nmagnetism threshold: %d\nplayer position: "
           "%.02f,%.02f\ngameover count: %d\npowerups onscreen: %d\n",
           config["target_texture_width"], config["target_texture_height"],
           config["window_width"], config["window_height"], current_frame_count,
           total_frame_count, entities.size(), fps(), zoom, num_collisions,
           knife_cooldown, current_knife_cooldown, num_knives, max_num_knives,
           num_knives_fired, knife_charge, num_enemies_escaped,
           get_num_enemies_killed(), player_health, player_max_health,
           player_soulshards, total_soulshards_collected,
           current_soulshard_magnetism_threshold, transforms[player_id].x,
           transforms[player_id].y, gameover_count, powerups_onscreen);
  text_surface = TTF_RenderText_Blended_Wrapped(gFont, texture_text, textColor,
                                                DEBUG_TEXT_WRAP_LEN);
  if (text_surface == nullptr) {
    mPrint("text_surface == nullptr");
    mPrint("Unable to render text_surface! SDL_ttf Error: " +
           string(TTF_GetError()));
  } else {
    // Create texture from surface pixels
    if (debug_texture != nullptr) {
      SDL_DestroyTexture(debug_texture);
      debug_texture = nullptr;
    }
    debug_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (debug_texture == nullptr) {
      mPrint("mTexture == NULL");
      mPrint("Unable to create texture from rendered text! SDL Error: " +
             string(SDL_GetError()));
    }
    // Get image dimensions
    mWidth = text_surface->w;
    mHeight = text_surface->h;
    // Get rid of old surface
    SDL_FreeSurface(text_surface);
    text_surface = nullptr;
  }
}

void load_texture(string key, string path, const int numclips) {
  assert(numclips > 0);
  SDL_Texture *t = IMG_LoadTexture(renderer, path.c_str());
  if (t == nullptr) {
    mPrint("Failed to load texture image with key and path: " + key + "," +
           path);
    cleanup_and_exit_with_failure();
  }
  textures[key] = t;
  num_clips[key] = numclips;
}

void load_texture_with_color_mod(string key, string path, const int numclips,
                                 Uint8 r, Uint8 g, Uint8 b) {
  SDL_Texture *t = IMG_LoadTexture(renderer, path.c_str());
  if (t == nullptr) {
    mPrint("Failed to load texture image with key and path: " + key + "," +
           path);
    cleanup_and_exit_with_failure();
  }
  SDL_SetTextureColorMod(t, r, g, b);
  textures[key] = t;
  num_clips[key] = numclips;
  // mPrint("load texture with color mod: " + key + "," + path + "," +
  //        to_string(r) + "," + to_string(g) + "," + to_string(b));
}

void load_pixel(string key, Uint8 r, Uint8 g, Uint8 b, int w, int h) {
  SDL_Texture *pixel = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_TARGET, w, h);
  if (pixel == nullptr) {
    mPrint("Failed to create pixel texture with key: " + key);
    cleanup_and_exit_with_failure();
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
  // string integer_keys[] = {"r", "g", "b", "numclips"};
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
  int numclips = 1;
  // int numclips = v["numclips"].GetInt();
  if (check_if_json_has_member_and_is_int(v, "numclips")) {
    numclips = v["numclips"].GetInt();
  }
  load_texture_with_color_mod(key, path, numclips, r, g, b);
}

void handle_load_texture(Value &v) {
  int clips = 1;
  if (!json_value_has_member_is_string(v, "path")) {
    string msg = "config/textures.json array element has no path";
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }
  if (!json_value_has_member_is_string(v, "key")) {
    string msg = "config/textures.json array element has no key";
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }
  if (check_if_json_has_member_and_is_int(v, "numclips")) {
    clips = v["numclips"].GetInt();
  }
  string path = v["path"].GetString();
  string key = v["key"].GetString();
  load_texture(key, path, clips);
}

const Document load_document(string path) {
  const size_t read_buffer_size = 65536;
  char readBuffer[read_buffer_size] = {0};
  Document d;
  FILE *fp = fopen(path.c_str(), "r");
  if (fp == nullptr) {
    mPrint("Failed to open " + path);
    cleanup_and_exit_with_failure();
  }
  fread(readBuffer, 1, read_buffer_size, fp);
  fclose(fp);
  d.Parse(readBuffer);
  if (d.HasParseError()) {
    string msg = "Failed to parse " + path;
    mPrint(msg);
    msg = "Error code: " + to_string(d.GetParseError());
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

void handle_load_texture_by_type(Value &v) {
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
  mPrint("begin load textures");
  // Document d = load_textures_document();
  Document d = load_document("config/textures.json");
  if (d.IsArray()) {
    for (auto &v : d.GetArray()) {
      check_if_json_value_is_object(v);
      check_if_json_value_has_member_and_is_string(v, "type");
      check_if_json_value_has_member_and_is_string(v, "key");
      handle_load_texture_by_type(v);
    }
  }
  load_gameover_texture();
  mPrint("end load textures");
}

void json_value_has_member_is_int_set_config(Document &d, string member) {
  if (!d.HasMember(member.c_str()) || !d[member.c_str()].IsInt()) {
    string msg = "config/game.json has no " + member;
    mPrint(msg);
    cleanup_and_exit_with_failure();
  }
  config[member] = d[member.c_str()].GetInt();
}

void load_main_config() {
  Document d = load_document("config/game.json");
  json_value_has_member_is_int_set_config(d, "debug_font_size");
  json_value_has_member_is_int_set_config(d, "default_window_width");
  json_value_has_member_is_int_set_config(d, "default_window_height");
  json_value_has_member_is_int_set_config(d, "blood_pixel_count");
  json_value_has_member_is_int_set_config(d, "default_magnetism_threshold");
  // copy over default window width and height as default window width values
  config["window_width"] = config["default_window_width"];
  config["window_height"] = config["default_window_height"];
  window_width = config["window_width"];
  window_height = config["window_height"];
  config["target_texture_width"] = config["default_window_width"];
  config["target_texture_height"] = config["default_window_height"];
  current_soulshard_magnetism_threshold = config["default_magnetism_threshold"];
}

void load_enemy_bullet_definitions() {
  Document d = load_document("config/enemy-bullets.json");

  string str_keys[] = {"type", "movement"};
  string int_keys[] = {"speed", "frequency", "cooldown"};
  string double_keys[] = {"scale"};

  if (d.IsArray()) {
    for (auto &v : d.GetArray()) {
      check_if_json_value_is_object(v);

      for (string key : str_keys) {
        if (!v.HasMember(key.c_str()) || !v[key.c_str()].IsString()) {
          string msg = "config/enemy-bullets.json array element has no " + key;
          mPrint(msg);
          cleanup_and_exit_with_failure();
        }
      }

      for (string key : int_keys) {
        if (!v.HasMember(key.c_str()) || !v[key.c_str()].IsInt()) {
          string msg = "config/enemy-bullets.json array element has no " + key;
          mPrint(msg);
          cleanup_and_exit_with_failure();
        }
      }

      for (string key : double_keys) {
        if (!v.HasMember(key.c_str()) || !v[key.c_str()].IsDouble()) {
          string msg = "config/enemy-bullets.json array element has no " + key;
          mPrint(msg);
          cleanup_and_exit_with_failure();
        }
      }

      string type_str = v["type"].GetString();
      enemy_type type = ENEMY_TYPE_EYEBALL;
      if (type_str == "eyeball") {
        type = ENEMY_TYPE_EYEBALL;
      } else if (type_str == "bat") {
        type = ENEMY_TYPE_BAT;
      } else if (type_str == "goblin") {
        type = ENEMY_TYPE_GOBLIN;
      } else {
        string msg = "config/enemy-bullets.json array element has invalid type";
        mPrint(msg);
        cleanup_and_exit_with_failure();
      }

      enemy_bullet_movement movement = ENEMY_BULLET_MOVEMENT_UP;
      string movement_str = v["movement"].GetString();
      if (movement_str == "up") {
        movement = ENEMY_BULLET_MOVEMENT_UP;
      } else if (movement_str == "towards-player") {
        movement = ENEMY_BULLET_MOVEMENT_TOWARDS_PLAYER;
      } else {
        string msg =
            "config/enemy-bullets.json array element has invalid movement";
        mPrint(msg);
        cleanup_and_exit_with_failure();
      }

      int speed = v["speed"].GetInt();
      int frequency = v["frequency"].GetInt();
      int cooldown = v["cooldown"].GetInt();
      double scale = v["scale"].GetDouble();

      enemy_bullet_definition def = {movement, speed, frequency, cooldown,
                                     scale};
      enemy_bullet_definitions[type] = def;
    }
  }
}

void load_generators() {
  Document d = load_document("config/generators.json");

  string int_keys[] = {"groupnum",    "cooldown",    "cooldown_reduction",
                       "spawn_count", "frame_begin", "hp"};
  string str_keys[] = {"type", "position"};
  string double_keys[] = {"scale"};

  if (d.IsArray()) {
    for (auto &v : d.GetArray()) {
      check_if_json_value_is_object(v);

      for (string key : int_keys) {
        if (!v.HasMember(key.c_str()) || !v[key.c_str()].IsInt()) {
          string msg = "config/generators.json array element has no " + key;
          mPrint(msg);
          cleanup_and_exit_with_failure();
        }
      }

      for (string key : str_keys) {
        if (!v.HasMember(key.c_str()) || !v[key.c_str()].IsString()) {
          string msg = "config/generators.json array element has no " + key;
          mPrint(msg);
          cleanup_and_exit_with_failure();
        }
      }

      for (string key : double_keys) {
        if (!v.HasMember(key.c_str()) || !v[key.c_str()].IsDouble()) {
          string msg = "config/generators.json array element has no " + key;
          mPrint(msg);
          cleanup_and_exit_with_failure();
        }
      }

      enemy_type type = ENEMY_TYPE_EYEBALL;
      string type_str = v["type"].GetString();
      if (type_str == "eyeball") {
        type = ENEMY_TYPE_EYEBALL;
      } else if (type_str == "bat") {
        type = ENEMY_TYPE_BAT;
      } else if (type_str == "goblin") {
        type = ENEMY_TYPE_GOBLIN;

      } else {
        string msg = "config/generators.json array element has invalid type";
        mPrint(msg);
        cleanup_and_exit_with_failure();
      }

      bool is_active = true;
      int groupnum = v["groupnum"].GetInt();
      int cooldown = v["cooldown"].GetInt();
      int cooldown_reduction = v["cooldown_reduction"].GetInt();
      int spawn_count = v["spawn_count"].GetInt();
      int frame_begin = v["frame_begin"].GetInt();
      int hp = v["hp"].GetInt();
      double scale = v["scale"].GetDouble();

      screen_position_t screen_position = SCREEN_POSITION_LEFT;
      string position = v["position"].GetString();
      if (position == "left") {
        screen_position = SCREEN_POSITION_LEFT;
      } else if (position == "right") {
        screen_position = SCREEN_POSITION_RIGHT;
      } else {
        string msg =
            "config/generators.json array element has invalid position";
        mPrint(msg);
        cleanup_and_exit_with_failure();
      }

      spawn_generator(type, is_active, groupnum, cooldown, cooldown_reduction,
                      frame_begin, spawn_count, hp, scale, screen_position);
    }
  }
}
