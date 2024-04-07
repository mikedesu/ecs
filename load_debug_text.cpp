
#include "SDL_handler.h"

#include <cstdio>
// #include <string>
#include "enemy_type.h"
#include "entity_id.h"
#include "mPrint.h"
#include <vector>

using std::snprintf;
// using std::string;
using std::vector;

extern int target_texture_width;
extern int target_texture_height;
extern int window_width;
extern int window_height;
extern int frame_count;
extern int num_collisions;
extern int knife_cooldown;
extern int current_knife_cooldown;
extern int num_knives_fired;
extern int num_enemies_escaped;
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

extern double fps();

extern size_t get_num_enemies_killed();

void load_debug_text() {
  snprintf(texture_text, 1024,
           "target texture: %dx%d\nwindow size: %dx%d\nframe_count: "
           "%06d\nnum_entities: %ld\n"
           "fps: %.02f\nzoom: %.02f\nnum_collisions: "
           "%d\nknife_cooldown: %d\ncurrent_knife_cooldown: "
           "%d\nnum_knives_fired: %d\nnum_enemies_escaped: "
           "%d\nnum_enemies_killed: %ld\n",
           target_texture_width, target_texture_height, window_width,
           window_height, frame_count, entities.size(), fps(), zoom,
           num_collisions, knife_cooldown, current_knife_cooldown,
           num_knives_fired, num_enemies_escaped, get_num_enemies_killed());
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
