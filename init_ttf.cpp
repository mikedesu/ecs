
#include "SDL_handler.h"
#include <string>

using std::string;

extern int result;

extern void cleanup_and_exit_with_failure_mprint(string msg);

void init_ttf() {
  result = TTF_Init();
  if (result == -1) {
    cleanup_and_exit_with_failure_mprint("Failed to init TTF" +
                                         string(TTF_GetError()));
  }
}
