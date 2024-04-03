#include "SDL_handler.h"
#include "mPrint.h"

#include <cmath>
#include <cstdio>
#include <ctime>
#include <string>
#include <unordered_map>
#include <vector>

using std::exit;
using std::pow;
using std::snprintf;
using std::sqrt;
using std::string;
using std::unordered_map;
using std::vector;

typedef int entity_id;

typedef struct {
  SDL_Texture *texture;
  SDL_Rect src;
  SDL_Rect dest;
  int current_clip;
  int num_clips;
  bool is_animating;
} sprite_component;

typedef struct {
  int x;
  int y;
} transform_component;

void load_debug_text();
void spawn_eyeball();
int generate_random_char();
entity_id get_next_entity_id();
double frame_time();
double fps();
void handle_input();
void handle_keydown();
void handle_keyup();
int init_target_texture();
void cleanup();
void cleanup_sprites();
void cleanup_transforms();
void cleanup_inputs();
void cleanup_entities();
void cleanup_textures();
void init_debug_texture_rects();
void init_target_texture_rects();
void cleanup_and_exit_with_failure();
void cleanup_and_exit_with_failure_mprint(string message);
void create_window();
void create_renderer();
void init_img();
void init_ttf();
void init_gfont();
void handle_init_target_texture();
void load_skull_sheet_texture();
void load_eyeball_sheet_texture();
void spawn_skull();
void handle_input_component();
void handle_transform_components();
void render_sprites();
void render_debug_panel();
void update_animations();
void render_frame();

const int DEBUG_TEXT_WRAP_LEN = 2048;
const int MIN_SPAWN_DISTANCE = 100;

char texture_text[1024] = "a bunch of random text";
const int target_texture_width = 400;
const int target_texture_height = 240;
double zoom = 4.0;
const int window_width = target_texture_width * zoom;
const int window_height = target_texture_height * zoom;
int imgFlags = IMG_INIT_PNG;
int result = -1;
int w = 0;
int h = 0;
int num_clips = 2;
int frame_count = 0;
int mWidth = -1;
int mHeight = -1;
bool quit = false;
bool do_render_debug_panel = true;
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
string skullsheet_filepath = "img/skull-sheet.png";
string eyeballsheet_filepath = "img/eyeball-sheet.png";
static entity_id next_entity_id = 0;
vector<entity_id> entities;
unordered_map<entity_id, sprite_component> sprites;
unordered_map<entity_id, transform_component> transforms;
unordered_map<entity_id, bool> inputs;
unordered_map<int, bool> is_pressed;
unordered_map<string, SDL_Texture *> textures;

int main() {
  srand(time(nullptr));
  SDL_Init(SDL_INIT_VIDEO);
  create_window();
  create_renderer();
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  init_img();
  init_ttf();
  handle_init_target_texture();
  load_skull_sheet_texture();
  load_eyeball_sheet_texture();
  init_gfont();
  load_debug_text();
  init_debug_texture_rects();
  // get the width and height of the texture
  init_target_texture_rects();
  spawn_skull();
  while (!quit) {
    handle_input();
    handle_input_component();
    // update game state
    handle_transform_components();
    update_animations();
    render_frame();
  }
  cleanup();
  return EXIT_SUCCESS;
}

void load_debug_text() {
  snprintf(texture_text, 1024,
           "target texture: %dx%d\nwindow size: %dx%d\nframe_count: "
           "%06d\nnum_entities: %d\n"
           "fps: %.02f\nzoom: %.02f",
           target_texture_width, target_texture_height, window_width,
           window_height, frame_count, next_entity_id, fps(), zoom);
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

entity_id get_next_entity_id() { return next_entity_id++; }

int distance(int x1, int y1, int x2, int y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

double frame_time() {
  static double last_time = 0;
  double current_time = SDL_GetTicks();
  double frame_time = current_time - last_time;
  last_time = current_time;
  return frame_time;
}

double fps() { return frame_count / (SDL_GetTicks64() / 1000.0f); }

void spawn_eyeball() {
  const int w = 14;
  const int h = 14;
  const int num_clips = 18;
  bool is_animating = true;
  entity_id id = get_next_entity_id();
  int x = rand() % (target_texture_width - w);
  int y = rand() % (target_texture_height - h);
  sprites[id] = {textures["eyeball"], {0, 0, w, h}, {0, 0, w, h}, 0,
                 num_clips,           is_animating};
  transforms[id] = {x, y};
  // inputs[id] = false;
  entities.push_back(id);
}

void handle_keydown() {
  switch (e.key.keysym.sym) {
  case SDLK_LEFT:
  case SDLK_RIGHT:
  case SDLK_UP:
  case SDLK_DOWN:
  case SDLK_a:
    is_pressed[e.key.keysym.sym] = true;
    break;
  case SDLK_q:
    quit = true;
    break;
  case SDLK_d:
    do_render_debug_panel = !do_render_debug_panel;
    break;
  case SDLK_i:
    for (int i = 0; i < 100; i++) {
      spawn_eyeball();
    }
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

void cleanup_sprites() {
  for (auto kv : sprites) {
    SDL_DestroyTexture(kv.second.texture);
  }
  sprites.clear();
}

void cleanup_entities() { entities.clear(); }

void cleanup_transforms() { transforms.clear(); }

void cleanup_inputs() { inputs.clear(); }

void cleanup_textures() {
  for (auto kv : textures) {
    SDL_DestroyTexture(kv.second);
  }
  textures.clear();
}

void cleanup() {
  mPrint("cleaning up");
  cleanup_sprites();
  cleanup_transforms();
  cleanup_inputs();
  cleanup_entities();
  cleanup_textures();
  SDL_DestroyTexture(debug_texture);
  SDL_DestroyTexture(debug_bg_texture);
  SDL_DestroyTexture(target_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(gFont);
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

void init_debug_texture_rects() {
  debug_texture_src.x = 0;
  debug_texture_src.y = 0;
  debug_texture_src.w = mWidth;
  debug_texture_src.h = mHeight;
  debug_texture_dest.x = 0;
  debug_texture_dest.y = 0;
  debug_texture_dest.w = mWidth;
  debug_texture_dest.h = mHeight;
}

void init_target_texture_rects() {
  target_texture_src.x = 0;
  target_texture_src.y = 0;
  target_texture_src.w = target_texture_width;
  target_texture_src.h = target_texture_height;
  target_texture_dest.x = 0;
  target_texture_dest.y = 0;
  target_texture_dest.w = window_width;
  target_texture_dest.h = window_height;
}

void cleanup_and_exit_with_failure() {
  cleanup();
  exit(EXIT_FAILURE);
}

void cleanup_and_exit_with_failure_mprint(string message) {
  mPrint(message);
  cleanup();
  exit(EXIT_FAILURE);
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
  result = IMG_Init(imgFlags);
  if ((result & imgFlags) != imgFlags) {
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
  gFont = TTF_OpenFont("ttf/hack.ttf", 28);
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

void spawn_skull() {
  SDL_QueryTexture(textures["skull"], NULL, NULL, &w, &h);
  entity_id id = get_next_entity_id();
  sprites[id] = {textures["skull"], {0, 0, 24, h}, {0, 0, 24, h}, 0, 2, false};
  transforms[id] = {0, 0};
  inputs[id] = true;
  entities.push_back(id);
}

void handle_input_component() {
  for (auto kv : inputs) {
    entity_id id = kv.first;
    transform_component transform = transforms[id];
    sprite_component sprite = sprites[id];
    if (is_pressed[SDLK_LEFT]) {
      transform.x--;
    } else if (is_pressed[SDLK_RIGHT]) {
      transform.x++;
    }
    if (is_pressed[SDLK_UP]) {
      transform.y--;
    } else if (is_pressed[SDLK_DOWN]) {
      transform.y++;
    }
    if (is_pressed[SDLK_a]) {
      sprite.current_clip = 1;
      sprite.src.x = sprite.current_clip * 24;
    } else {
      sprite.current_clip = 0;
      sprite.src.x = sprite.current_clip * 24;
    }
    transforms[id] = transform;
    sprites[id] = sprite;
  }
}

void handle_transform_components() {
  for (auto kv : transforms) {
    entity_id id = kv.first;
    transform_component transform = kv.second;
    sprite_component sprite = sprites[id];
    sprite.dest.x = kv.second.x;
    sprite.dest.y = kv.second.y;
    sprites[id] = sprite;
  }
}

void render_sprites() {
  for (auto id : entities) {
    sprite_component sprite = sprites[id];
    // transform_component transform = transforms[id];
    SDL_RenderCopy(renderer, sprite.texture, &sprite.src, &sprite.dest);
  }
}

void render_debug_panel() {
  SDL_Color color = {0, 0, 0, 128};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &debug_texture_dest);
  SDL_RenderCopy(renderer, debug_texture, &debug_texture_src,
                 &debug_texture_dest);
}

void update_animations() {
  for (auto id : entities) {
    sprite_component sprite = sprites[id];
    if (sprite.is_animating) {
      //   mPrint("animating");
      sprite.current_clip = (sprite.current_clip + 1) % sprite.num_clips;
      sprite.src.x = sprite.current_clip * 14;
      sprites[id] = sprite;
    }
  }
}

void render_frame() {
  // clear the target texture
  SDL_SetRenderTarget(renderer, target_texture);
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderClear(renderer);
  render_sprites();
  //  reset the render target
  SDL_SetRenderTarget(renderer, NULL);
  SDL_RenderCopy(renderer, target_texture, &target_texture_src,
                 &target_texture_dest);
  // render debug text
  if (do_render_debug_panel) {
    render_debug_panel();
    load_debug_text();
  }
  SDL_RenderPresent(renderer);
  frame_count++;
}
