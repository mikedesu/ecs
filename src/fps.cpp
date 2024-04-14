
#include "SDL_handler.h"
extern unsigned int frame_count;

double fps() { return frame_count / (SDL_GetTicks64() / 1000.0f); }
