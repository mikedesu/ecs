all: game

CC=clang++
OBJ_FLAG=-c
CFLAGS=-Wall -Wextra -Werror -Wpedantic -std=c++17

OBJ_FILES=distance.o \
					cleanup.o \
					create_renderer.o \
					create_window.o \
					fps.o \
					frame_time.o \
					get_next_entity_id.o \
					get_num_enemies_killed.o \
					generator_set_active.o \
					handle_input_component.o \
					handle_input.o \
					init_rng.o \
					init_debug_texture_rects.o \
					init_target_texture_rects.o \
					init_target_texture.o \
					init_gfont.o \
					init_ttf.o \
					init_img.o \
					load.o \
					render_debug_panel.o \
					render_frame.o \
					render_sprites.o \
					spawn.o \
					toggle_fullscreen.o \
					update.o \
					update_transform_components.o 

MAIN_FILE=main.cpp
SDL_FLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
BINARY=game

# main game binary
$(BINARY): $(MAIN_FILE) $(OBJ_FILES)
	$(CC) $^ $(CFLAGS) $(SDL_FLAGS) -lm -o $@

# object files
cleanup.o: cleanup.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
create_renderer.o: create_renderer.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
create_window.o: create_window.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
distance.o: distance.cpp
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
handle_input_component.o: handle_input_component.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
handle_input.o: handle_input.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
init_debug_texture_rects.o: init_debug_texture_rects.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
init_rng.o: init_rng.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
init_target_texture.o: init_target_texture.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
init_target_texture_rects.o: init_target_texture_rects.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
init_gfont.o: init_gfont.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
init_img.o: init_img.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
init_ttf.o: init_ttf.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
load.o: load.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
render_frame.o: render_frame.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
render_sprites.o: render_sprites.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
render_debug_panel.o: render_debug_panel.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
spawn.o: spawn.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
toggle_fullscreen.o: toggle_fullscreen.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
update_transform_components.o: update_transform_components.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^
update.o: update.cpp
	$(CC) $(CFLAGS) $(OBJ_FLAG) $^

clean:
	rm -f $(OBJ_FILES) $(BINARY)

