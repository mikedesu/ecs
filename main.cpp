#include "SDL_handler.h"

#include <string>

using std::string;

int main() {
  const int target_texture_width = 400;
  const int target_texture_height = 240;
  const int zoom = 4;
  const int window_width = target_texture_width * zoom;
  const int window_height = target_texture_height * zoom;

  SDL_Event e;

  int imgFlags = IMG_INIT_PNG;
  int result = -1;
  int w = 0;
  int h = 0;
  int num_clips = 2;

  bool quit = false;
  bool left_is_pressed = false;
  bool right_is_pressed = false;
  bool up_is_pressed = false;
  bool down_is_pressed = false;

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_Texture *target_texture = nullptr;
  SDL_Texture *skull_sheet_texture = nullptr;
  SDL_Rect skull_clip;
  SDL_Rect skull_dest;
  SDL_Rect target_texture_src;
  SDL_Rect target_texture_dest;

  string filepath = "img/skull-sheet.png";

  SDL_Init(SDL_INIT_VIDEO);
  window =
      SDL_CreateWindow("SDL2 Displaying Image", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  result = IMG_Init(imgFlags);
  // init the target texture
  target_texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
      target_texture_width, target_texture_height);
  skull_sheet_texture = IMG_LoadTexture(renderer, filepath.c_str());
  if (skull_sheet_texture == nullptr) {
    printf("Failed to load texture image!\n");
    return 1;
  }
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
    //  reset the render target
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, target_texture, &target_texture_src,
                   &target_texture_dest);
    SDL_RenderPresent(renderer);
  }
  return 0;
}
