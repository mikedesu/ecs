#include "SDL_handler.h"
#include <string>

using std::string;

extern TTF_Font *gFont;

extern int debug_font_size;

extern void cleanup_and_exit_with_failure_mprint(string message);

void init_gfont() {
  gFont = TTF_OpenFont("ttf/hack.ttf", debug_font_size);
  if (gFont == nullptr) {
    cleanup_and_exit_with_failure_mprint("Failed to load font");
  }
}
