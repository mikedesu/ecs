#include "SDL_handler.h"

double frame_time() {
  static double last_time = 0;
  double current_time = SDL_GetTicks();
  double frame_time = current_time - last_time;
  last_time = current_time;
  return frame_time;
}
