
#include "SDL_handler.h"
#include "mPrint.h"
#include <string>

using std::to_string;

extern bool is_fullscreen;
extern int fullscreen_width;
extern int fullscreen_height;
extern int window_width;
extern int window_height;
extern int target_texture_width;
extern int target_texture_height;
extern SDL_Rect target_texture_dest;

extern SDL_Window *window;

void toggle_fullscreen() {
  is_fullscreen = !is_fullscreen;
  SDL_SetWindowFullscreen(window,
                          is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

  if (is_fullscreen) {
    // what does the below line do?
    // SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_GetWindowSize(window, &fullscreen_width, &fullscreen_height);

    mPrint("fullscreen width: " + to_string(fullscreen_width));
    mPrint("fullscreen height: " + to_string(fullscreen_height));
    mPrint("window width: " + to_string(window_width));
    mPrint("window height: " + to_string(window_height));
    mPrint("texture width: " + to_string(target_texture_width));
    mPrint("texture height: " + to_string(target_texture_height));

    target_texture_dest.w = fullscreen_width;
    target_texture_dest.h = fullscreen_height;
  } else {
    target_texture_dest.w = window_width;
    target_texture_dest.h = window_height;
  }
}
