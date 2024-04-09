all: game

CC=clang++
OBJ_FLAG=-c
CFLAGS=-Wall -Wextra -Werror -Wpedantic -std=c++17

OBJ_FILES=cleanup.o \
					create.o \
					fps.o \
					frame_time.o \
					get_next_entity_id.o \
					get_num_enemies_killed.o \
					generator_set_active.o \
					handle_input.o \
					init.o \
					load.o \
					render.o \
					spawn.o \
					toggle_fullscreen.o \
					update.o \

MAIN_FILE=main.cpp
SDL_FLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
BINARY=game

# main game binary
$(BINARY): $(MAIN_FILE) $(OBJ_FILES)
	$(CC) $^ $(CFLAGS) $(SDL_FLAGS) -lm -o $@

# object files
cleanup.o: cleanup.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
create.o: create.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
fps.o: fps.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
frame_time.o: frame_time.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
generator_set_active.o: generator_set_active.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
get_next_entity_id.o: get_next_entity_id.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
get_num_enemies_killed.o: get_num_enemies_killed.cpp
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
toggle_fullscreen.o: toggle_fullscreen.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
update.o: update.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^

clean:
	rm -f $(OBJ_FILES) $(BINARY)

