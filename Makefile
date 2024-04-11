all: game

CC=clang++
OBJ_FLAG=-c
CFLAGS=-Wall -Wextra -Werror -Wpedantic -std=c++17

OBJ_FILES=bg.o \
					cleanup.o \
					create.o \
					fps.o \
					handle_input.o \
					init.o \
					load.o \
					render.o \
					spawn.o \
					tools.o \
					update.o \

MAIN_FILE=main.cpp
SDL_FLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
BINARY=game

# main game binary
$(BINARY): $(MAIN_FILE) $(OBJ_FILES)
	$(CC) $^ $(CFLAGS) $(SDL_FLAGS) -lm -o $@

# object files
bg.o: bg.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
cleanup.o: cleanup.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
create.o: create.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
fps.o: fps.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
handle_input.o: handle_input.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
init.o: init.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
load.o: load.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
render.o: render.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
spawn.o: spawn.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
tools.o: tools.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
update.o: update.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^

clean:
	rm -f $(OBJ_FILES) $(BINARY)

