#include "SDL_handler.h"
#include "mPrint.h"

#include <cstdio>
#include <string>

using std::snprintf;
using std::string;

#define DEBUG_TEXT_WRAP_LEN 2048

char texture_text[128] = "frame_count: 00000";
const int target_texture_width = 400;
const int target_texture_height = 240;
const int zoom = 4;
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
bool left_is_pressed = false;
bool right_is_pressed = false;
bool up_is_pressed = false;
bool down_is_pressed = false;
TTF_Font *gFont = nullptr;
SDL_Event e;
SDL_Surface *text_surface = nullptr;
SDL_Color textColor = {255, 255, 255};
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *target_texture = nullptr;
SDL_Texture *skull_sheet_texture = nullptr;
SDL_Texture *debug_texture = nullptr;
SDL_Rect skull_clip;
SDL_Rect skull_dest;
SDL_Rect target_texture_src;
SDL_Rect target_texture_dest;
SDL_Rect debug_texture_src;
SDL_Rect debug_texture_dest;
string filepath = "img/skull-sheet.png";

void load_debug_text();
void load_debug_text() {
  snprintf(texture_text, 128, "frame_count: %05d", frame_count++);
  text_surface = TTF_RenderText_Blended_Wrapped(gFont, texture_text, textColor,
                                                DEBUG_TEXT_WRAP_LEN);
  if (text_surface == NULL) {
    mPrint("text_surface == NULL");
    // printf("textureText = %s\n", textureText);
    printf("Unable to render text_surface! SDL_ttf Error: %s\n",
           TTF_GetError());
  } else {
    // Create texture from surface pixels
    debug_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (debug_texture == NULL) {
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

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  window =
      SDL_CreateWindow("SDL2 Displaying Image", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  result = IMG_Init(imgFlags);
  result = TTF_Init();
  // init the target texture
  target_texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      target_texture_width, target_texture_height);
  skull_sheet_texture = IMG_LoadTexture(renderer, filepath.c_str());
  if (skull_sheet_texture == nullptr) {
    printf("Failed to load texture image!\n");
    return 1;
  }

  gFont = TTF_OpenFont("ttf/hack.ttf", 28);
  // text_surface = TTF_RenderText_Blended_Wrapped(gFont, texture_text,
  // textColor,
  //                                               DEBUG_TEXT_WRAP_LEN);
  // if (text_surface == NULL) {
  //   mPrint("text_surface == NULL");
  //   // printf("textureText = %s\n", textureText);
  //   printf("Unable to render text_surface! SDL_ttf Error: %s\n",
  //          TTF_GetError());
  // } else {
  //   // Create texture from surface pixels
  //   debug_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
  //   if (debug_texture == NULL) {
  //     mPrint("mTexture == NULL");
  //     printf("Unable to create texture from rendered text! SDL Error: %s\n",
  //            SDL_GetError());
  //   }
  //   // Get image dimensions
  //   mWidth = text_surface->w;
  //   mHeight = text_surface->h;
  //   // Get rid of old surface
  //   SDL_FreeSurface(text_surface);
  // }

  load_debug_text();

  debug_texture_src.x = 0;
  debug_texture_src.y = 0;
  debug_texture_src.w = mWidth;
  debug_texture_src.h = mHeight;
  debug_texture_dest.x = 0;
  debug_texture_dest.y = 0;
  debug_texture_dest.w = mWidth;
  debug_texture_dest.h = mHeight;
  // get the width and height of the texture
  SDL_QueryTexture(skull_sheet_texture, NULL, NULL, &w, &h);
  skull_clip.x = 0;
  skull_clip.y = 0;
  skull_clip.w = 24; // has to be the size of one sprite animation
  skull_clip.h = h;  // has to be the size of the sprite sheet
  skull_dest.x = 0;
  skull_dest.y = 0;
  skull_dest.w = 24;
  skull_dest.h = h;
  target_texture_src.x = 0;
  target_texture_src.y = 0;
  target_texture_src.w = target_texture_width;
  target_texture_src.h = target_texture_height;
  target_texture_dest.x = 0;
  target_texture_dest.y = 0;
  target_texture_dest.w = window_width;
  target_texture_dest.h = window_height;

  while (!quit) {
    // handle input
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT:
          left_is_pressed = true;
          break;
        case SDLK_RIGHT:
          right_is_pressed = true;
          break;
        case SDLK_UP:
          up_is_pressed = true;
          break;
        case SDLK_DOWN:
          down_is_pressed = true;
          break;
        case SDLK_q:
          quit = true;
          break;
        default:
          break;
        }
      } else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT:
          left_is_pressed = false;
          break;
        case SDLK_RIGHT:
          right_is_pressed = false;
          break;
        case SDLK_UP:
          up_is_pressed = false;
          break;
        case SDLK_DOWN:
          down_is_pressed = false;
          break;
        default:
          break;
        }
      }
    }
    // update gamestate
    if (left_is_pressed) {
      skull_dest.x--;
    } else if (right_is_pressed) {
      skull_dest.x++;
    }
    if (up_is_pressed) {
      skull_dest.y--;
    } else if (down_is_pressed) {
      skull_dest.y++;
    }
    // render frame
    // clear screen
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    // clear the target texture
    SDL_SetRenderTarget(renderer, target_texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, skull_sheet_texture, &skull_clip, &skull_dest);
    // render debug text
    // SDL_RenderCopy(renderer, debug_texture, NULL, NULL); // lol
    //  reset the render target
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, target_texture, &target_texture_src,
                   &target_texture_dest);
    SDL_RenderCopy(renderer, debug_texture, &debug_texture_src,
                   &debug_texture_dest);
    SDL_RenderPresent(renderer);
    // lets fuck with the text

    load_debug_text();
  }
  return 0;
}
