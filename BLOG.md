# BLOG.md

## 2024 April 08 

### 00:39 CST

- While checking out any memory leaks via `valgrind`, I discovered that `libsdl2` may be leaking memory in the library itself.
    - There are a number of calls to `malloc` during `SDL_Init()` but the memory is still available on exit
    - The `SDL_Init()` function calls different libraries depending on what flags are passed
        - `SDL_INIT_VIDEO`
            - `XSetLocaleModifiers (in /usr/lib/x86_64-linux-gnu/libX11.so.6.4.0)`
        - `SDL_INIT_TIMER`
        - `0`
            - `_dbus_string_copy_len (in /usr/lib/x86_64-linux-gnu/libdbus-1.so.3.32.4)`
            - `dbus_bus_register (in /usr/lib/x86_64-linux-gnu/libdbus-1.so.3.32.4)`
