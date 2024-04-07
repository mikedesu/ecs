#include "SDL_handler.h"
#include "enemy_type.h"
#include "entity_id.h"
#include "generator_component.h"
#include "mPrint.h"
#include "sprite_component.h"
#include "transform_component.h"

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

using std::default_random_engine;
using std::exit;
using std::remove;
using std::snprintf;
using std::string;
using std::uniform_real_distribution;
using std::unordered_map;
using std::vector;

const int DEBUG_TEXT_WRAP_LEN = 2048;
const int MIN_SPAWN_DISTANCE = 100;

double zoom = 1.0; // has to appear
char texture_text[1024] = "a bunch of random text";
int target_texture_width = 1600;
int target_texture_height = 960;
const int debug_font_size = 24;
const int default_window_width = 1600;
const int default_window_height = 960;
const int default_knife_speed = 4;
const int default_knife_cooldown = 10;
int window_width = default_window_width;
int window_height = default_window_height;
static int knife_cooldown = 0;
bool quit = false;
bool do_render_debug_panel = true;
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
int num_knives_fired = 0;
int num_enemies_escaped = 0;
string skullsheet_filepath = "img/skull-sheet4x.png";
string eyeballsheet_filepath = "img/eyeball-sheet4x.png";
entity_id next_entity_id = 0;
entity_id player_id = -1;
TTF_Font *gFont = nullptr;
SDL_Event e;
SDL_Surface *text_surface = nullptr;
SDL_Color textColor = {255, 255, 255};
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *target_texture = nullptr;
SDL_Texture *debug_texture = nullptr;
SDL_Texture *debug_bg_texture = nullptr;
SDL_Rect target_texture_src;
SDL_Rect target_texture_dest;
SDL_Rect debug_texture_src;
SDL_Rect debug_texture_dest;
SDL_Surface *debug_surface = nullptr;
vector<entity_id> entities;
vector<entity_id> entities_marked_for_deletion_tmp;
unordered_map<entity_id, sprite_component> sprites;
unordered_map<entity_id, transform_component> transforms;
unordered_map<entity_id, bool> inputs;
unordered_map<int, bool> is_pressed;
unordered_map<string, SDL_Texture *> textures;
unordered_map<entity_id, bool> is_rotating;
unordered_map<entity_id, bool> is_collidable;
unordered_map<entity_id, bool> is_enemy;
unordered_map<entity_id, bool> is_knife;
unordered_map<entity_id, bool> is_marked_for_deletion;
unordered_map<entity_id, generator_component> generators;

// random number generator
default_random_engine rng_generator;
uniform_real_distribution<double> eyeball_vx_distribution;

// externally defined
double fps();
double frame_time();
double distance(int x1, int y1, int x2, int y2);
void render_sprites();
void render_frame();
void init_rng();
void update_rotations();
void spawn_generator(enemy_type type, int timer, int cooldown);
void update_transform_components();
void update_generators();
void update_animations();
void update_knife_collisions();
void init_debug_texture_rects();
void init_target_texture_rects();
void render_debug_panel();
entity_id get_next_entity_id();

// internally defined
int init_target_texture();
void cleanup();
void cleanup_and_exit_with_failure();
void cleanup_and_exit_with_failure_mprint(string message);
void cleanup_entities_marked_for_deletion();
void cleanup_textures();
void create_window();
void create_renderer();
void handle_input();
void handle_init_target_texture();
void handle_input_component();
void handle_keydown();
void handle_keyup();
void init_gfont();
void init_img();
void init_ttf();
void load_debug_text();
void load_skull_sheet_texture();
void load_eyeball_sheet_texture();
void load_knife_sheet_texture();
void spawn_eyeball();
void spawn_knife();
void spawn_skull();

int main() {
  srand(time(nullptr));
  init_rng();
  SDL_Init(SDL_INIT_VIDEO);
  create_window();
  create_renderer();
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  init_img();
  init_ttf();
  handle_init_target_texture();
  load_skull_sheet_texture();
  load_eyeball_sheet_texture();
  load_knife_sheet_texture();
  init_gfont();
  load_debug_text();
  init_debug_texture_rects();
  // get the width and height of the texture
  init_target_texture_rects();
  spawn_skull();
  spawn_generator(ENEMY_TYPE_EYEBALL, 0, 120);

  while (!quit) {
    handle_input();
    handle_input_component();
    // update game state
    update_transform_components();
    update_animations();
    update_rotations();
    update_knife_collisions();
    update_generators();
    render_frame();
    knife_cooldown = (knife_cooldown > 0) ? knife_cooldown - 1 : 0;
    // remove entities that are marked for deletion
    cleanup_entities_marked_for_deletion();
  }
  cleanup();
  return EXIT_SUCCESS;
}

void cleanup_entities_marked_for_deletion() {
  for (auto kv : is_marked_for_deletion) {
    entity_id id = kv.first;
    if (kv.second) {
      sprites.erase(id);
      transforms.erase(id);
      inputs.erase(id);
      is_rotating.erase(id);
      is_collidable.erase(id);
      is_enemy.erase(id);
      is_knife.erase(id);
      entities.erase(remove(entities.begin(), entities.end(), id),
                     entities.end());
      entities_marked_for_deletion_tmp.push_back(id);
    }
  }
  for (auto id : entities_marked_for_deletion_tmp) {
    is_marked_for_deletion.erase(id);
  }
  entities_marked_for_deletion_tmp.clear();
}

void load_debug_text() {
  snprintf(texture_text, 1024,
           "target texture: %dx%d\nwindow size: %dx%d\nframe_count: "
           "%06d\nnum_entities: %ld\n"
           "fps: %.02f\nzoom: %.02f\nnum_collisions: "
           "%d\nknife_cooldown: %d\ncurrent_knife_cooldown: "
           "%d\nnum_knives_fired: %d\nnum_enemies_escaped: %d\n",
           target_texture_width, target_texture_height, window_width,
           window_height, frame_count, entities.size(), fps(), zoom,
           num_collisions, knife_cooldown, current_knife_cooldown,
           num_knives_fired, num_enemies_escaped);
  text_surface = TTF_RenderText_Blended_Wrapped(gFont, texture_text, textColor,
                                                DEBUG_TEXT_WRAP_LEN);
  if (text_surface == NULL) {
    mPrint("text_surface == NULL");
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
  }
}

void spawn_eyeball() {
  const int num_clips = 18;
  bool is_animating = true;
  SDL_QueryTexture(textures["eyeball"], NULL, NULL, &w, &h);
  w = w / num_clips;
  entity_id id = get_next_entity_id();
  double x = (target_texture_width - w);
  double y = (rand() % (target_texture_height - h));
  double vy = 0.0;
  double vx = eyeball_vx_distribution(rng_generator);
  // double vy = distribution(generator);
  double angle = 0.0;
  sprites[id] = {is_animating, 0,
                 num_clips,    textures["eyeball"],
                 {0, 0, w, h}, {(int)x, (int)y, w, h}};
  transforms[id] = {x, y, vx, vy, angle};
  is_collidable[id] = true;
  is_enemy[id] = true;
  entities.push_back(id);
}

void spawn_knife() {
  if (!knife_cooldown) {
    const int num_clips = 1;
    const int padding = 16;
    bool is_animating = false;
    entity_id id = get_next_entity_id();
    // mPrint("spawning knife with id " + std::to_string(id));
    SDL_QueryTexture(textures["knife"], NULL, NULL, &w, &h);
    w = w / num_clips;
    double x = sprites[player_id].dest.x + sprites[player_id].dest.w + padding;
    double y = sprites[player_id].dest.y + sprites[player_id].dest.h / 2.0 + 4;
    double angle = 0.0;
    sprites[id] = {is_animating, 0,           num_clips, textures["knife"],
                   {0, 0, w, h}, {0, 0, w, h}};
    double vx = current_knife_speed;
    double vy = 0;
    transforms[id] = {x, y, vx, vy, angle};
    is_knife[id] = true;
    is_enemy[id] = false;
    is_collidable[id] = true;
    is_rotating[id] = true;
    entities.push_back(id);
    knife_cooldown = current_knife_cooldown;
    num_knives_fired++;
  }
}

void handle_keydown() {
  switch (e.key.keysym.sym) {
  case SDLK_LEFT:
  case SDLK_RIGHT:
  case SDLK_UP:
  case SDLK_DOWN:
  case SDLK_a:
  case SDLK_z:
  case SDLK_x:
  case SDLK_LSHIFT:
  case SDLK_RSHIFT:
    is_pressed[e.key.keysym.sym] = true;
    break;
  case SDLK_q:
    quit = true;
    break;
  case SDLK_d:
    do_render_debug_panel = !do_render_debug_panel;
    break;
  default:
    break;
  }
}

void handle_keyup() {
  switch (e.key.keysym.sym) {
  case SDLK_LEFT:
  case SDLK_RIGHT:
  case SDLK_UP:
  case SDLK_DOWN:
  case SDLK_a:
  case SDLK_z:
  case SDLK_x:
  case SDLK_LSHIFT:
  case SDLK_RSHIFT:
    is_pressed[e.key.keysym.sym] = false;
    break;
  default:
    break;
  }
}

void handle_input() {
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      quit = true;
    } else if (e.type == SDL_KEYDOWN) {
      handle_keydown();
    } else if (e.type == SDL_KEYUP) {
      handle_keyup();
    }
  }
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

void create_window() {
  window =
      SDL_CreateWindow("SDL2 Displaying Image", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
  if (window == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to create window: " +
                                         string(SDL_GetError()));
  }
}

void create_renderer() {
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr) {
    cleanup_and_exit_with_failure_mprint("Could not create renderer: " +
                                         string(SDL_GetError()));
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

void init_ttf() {
  result = TTF_Init();
  if (result == -1) {
    cleanup_and_exit_with_failure_mprint("Failed to init TTF" +
                                         string(TTF_GetError()));
  }
}

void init_gfont() {
  gFont = TTF_OpenFont("ttf/hack.ttf", debug_font_size);
  if (gFont == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load font");
  }
}

void handle_init_target_texture() {
  result = init_target_texture();
  if (!result) {
    cleanup_and_exit_with_failure_mprint("Failed to init target texture");
  }
}

void load_skull_sheet_texture() {
  SDL_Texture *t = IMG_LoadTexture(renderer, skullsheet_filepath.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         skullsheet_filepath);
  }
  textures["skull"] = t;
}

void load_eyeball_sheet_texture() {
  SDL_Texture *t = IMG_LoadTexture(renderer, eyeballsheet_filepath.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         eyeballsheet_filepath);
  }
  textures["eyeball"] = t;
}

void load_knife_sheet_texture() {
  string filepath = "img/knife4x.png";
  SDL_Texture *t = IMG_LoadTexture(renderer, filepath.c_str());
  if (t == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load texture image: " +
                                         filepath);
  }
  textures["knife"] = t;
}

void spawn_skull() {
  if (player_id == -1) {
    const int num_clips = 2;
    bool is_animating = false;
    int src_x = 0;
    int src_y = 0;
    double dest_x = 0;
    double dest_y = 0;
    double vx = 0;
    double vy = 0;
    double angle = 0.0;
    SDL_QueryTexture(textures["skull"], NULL, NULL, &w, &h);
    w = w / num_clips;
    entity_id id = get_next_entity_id();
    sprites[id] = {is_animating,
                   0,
                   num_clips,
                   textures["skull"],
                   {src_x, src_y, w, h},
                   {(int)dest_x, (int)dest_y, w, h}};
    transforms[id] = {dest_x, dest_y, vx, vy, angle};
    inputs[id] = true;
    player_id = id;
    entities.push_back(id);
  }
}

void handle_input_component() {
  for (auto kv : inputs) {
    entity_id id = kv.first;
    transform_component transform = transforms[id];
    sprite_component sprite = sprites[id];

    if (is_pressed[SDLK_LEFT] && is_pressed[SDLK_LSHIFT]) {
      transform.x -= 8;
    } else if (is_pressed[SDLK_RIGHT] && is_pressed[SDLK_LSHIFT]) {
      transform.x += 8;
    } else if (is_pressed[SDLK_LEFT]) {
      transform.x -= 4;
    } else if (is_pressed[SDLK_RIGHT]) {
      transform.x += 4;
    }

    if (is_pressed[SDLK_UP] && is_pressed[SDLK_LSHIFT]) {
      transform.y -= 8;
    } else if (is_pressed[SDLK_DOWN] && is_pressed[SDLK_LSHIFT]) {
      transform.y += 8;
    } else if (is_pressed[SDLK_UP]) {
      transform.y -= 4;
    } else if (is_pressed[SDLK_DOWN]) {
      transform.y += 4;
    }

    if (is_pressed[SDLK_a]) {
      spawn_knife();
      sprite.current_clip = 1;
      sprite.src.x = sprite.current_clip * sprite.src.w;
    } else {
      sprite.current_clip = 0;
      sprite.src.x = sprite.current_clip * sprite.src.w;
    }
    transforms[id] = transform;
    sprites[id] = sprite;
  }
}
