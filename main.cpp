#include "SDL_handler.h"
#include <string>
using std::string;

int main() {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  // SDL_Texture *texture = nullptr;
  // SDL_Surface *surface = nullptr;
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("SDL2 Displaying Image", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  // surface = SDL_LoadPNG("image.png");
  // texture = SDL_CreateTextureFromSurface(renderer, surface);
  int imgFlags = IMG_INIT_PNG;
  int result = IMG_Init(imgFlags);
  SDL_Event e;
  bool quit = false;
  string filepath = "img/skull-sheet.png";

  SDL_Texture *skull_sheet_texture =
      IMG_LoadTexture(renderer, filepath.c_str());
  if (skull_sheet_texture == nullptr) {
    printf("Failed to load texture image!\n");
    return 1;
  }
  // get the width and height of the texture
  int w = 0;
  int h = 0;
  SDL_QueryTexture(skull_sheet_texture, NULL, NULL, &w, &h);

  SDL_Rect skull_clip;
  skull_clip.x = 0;
  skull_clip.y = 0;
  skull_clip.w = 24; // has to be the size of one sprite animation
  skull_clip.h = h;  // has to be the size of the sprite sheet
  int num_clips = 2;

  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);

    // render the texture
    for (int i = 0; i < num_clips; i++) {
      skull_clip.x = i * 24;
      SDL_RenderCopy(renderer, skull_sheet_texture, &skull_clip, NULL);
    }

    SDL_RenderPresent(renderer);
  }
  return 0;
}
