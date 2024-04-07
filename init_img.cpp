#include "SDL_handler.h"
#include <string>
using std::string;
extern int img_flags;
extern int result;
extern void cleanup_and_exit_with_failure_mprint(string msg);

void init_img() {
  result = IMG_Init(img_flags);
  if ((result & img_flags) != img_flags) {
    cleanup_and_exit_with_failure_mprint(
        "IMG_Init: Failed to init required png support: " +
        string(IMG_GetError()));
  }
}
