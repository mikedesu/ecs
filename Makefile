all: game

CC=clang++
OBJ_FLAG=-c

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
					load_knife_sheet_texture.o \
					load_eyeball_sheet_texture.o \
					load_skull_sheet_texture.o \
					load_coin_sheet_texture.o \
					load_debug_text.o \
					render_debug_panel.o \
					render_frame.o \
					render_sprites.o \
					spawn_generator.o \
					spawn_skull.o \
					spawn_knife.o \
					spawn_eyeball.o \
					spawn_coin.o \
					toggle_fullscreen.o \
					update_animations.o \
					update_collisions.o \
					update_generators.o \
					update_knife_collisions.o \
					update_rotations.o \
					update_transform_components.o 

MAIN_FILE=main.cpp
SDL_FLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
BINARY=game

# main game binary
$(BINARY): $(MAIN_FILE) $(OBJ_FILES)
	$(CC) $^ $(SDL_FLAGS) -lm -o $@

# object files
cleanup.o: cleanup.cpp
	$(CC) $(OBJ_FLAG) $^
create_renderer.o: create_renderer.cpp
	$(CC) $(OBJ_FLAG) $^
create_window.o: create_window.cpp
	$(CC) $(OBJ_FLAG) $^
distance.o: distance.cpp
	$(CC) $(OBJ_FLAG) $^
fps.o: fps.cpp
	$(CC) $(OBJ_FLAG) $^
frame_time.o: frame_time.cpp
	$(CC) $(OBJ_FLAG) $^
generator_set_active.o: generator_set_active.cpp
	$(CC) $(OBJ_FLAG) $^
get_next_entity_id.o: get_next_entity_id.cpp
	$(CC) $(OBJ_FLAG) $^
get_num_enemies_killed.o: get_num_enemies_killed.cpp
	$(CC) $(OBJ_FLAG) $^
handle_input_component.o: handle_input_component.cpp
	$(CC) $(OBJ_FLAG) $^
handle_input.o: handle_input.cpp
	$(CC) $(OBJ_FLAG) $^
init_debug_texture_rects.o: init_debug_texture_rects.cpp
	$(CC) $(OBJ_FLAG) $^
init_rng.o: init_rng.cpp
	$(CC) $(OBJ_FLAG) $^
init_target_texture.o: init_target_texture.cpp
	$(CC) $(OBJ_FLAG) $^
init_target_texture_rects.o: init_target_texture_rects.cpp
	$(CC) $(OBJ_FLAG) $^
init_gfont.o: init_gfont.cpp
	$(CC) $(OBJ_FLAG) $^
init_img.o: init_img.cpp
	$(CC) $(OBJ_FLAG) $^
init_ttf.o: init_ttf.cpp
	$(CC) $(OBJ_FLAG) $^
load_coin_sheet_texture.o: load_coin_sheet_texture.cpp
	$(CC) $(OBJ_FLAG) $^
load_debug_text.o: load_debug_text.cpp
	$(CC) $(OBJ_FLAG) $^
load_eyeball_sheet_texture.o: load_eyeball_sheet_texture.cpp
	$(CC) $(OBJ_FLAG) $^
load_knife_sheet_texture.o: load_knife_sheet_texture.cpp
	$(CC) $(OBJ_FLAG) $^
load_skull_sheet_texture.o: load_skull_sheet_texture.cpp
	$(CC) $(OBJ_FLAG) $^
render_frame.o: render_frame.cpp
	$(CC) $(OBJ_FLAG) $^
render_sprites.o: render_sprites.cpp
	$(CC) $(OBJ_FLAG) $^
render_debug_panel.o: render_debug_panel.cpp
	$(CC) $(OBJ_FLAG) $^
spawn_coin.o: spawn_coin.cpp
	$(CC) $(OBJ_FLAG) $^
spawn_eyeball.o: spawn_eyeball.cpp
	$(CC) $(OBJ_FLAG) $^
spawn_generator.o: spawn_generator.cpp
	$(CC) $(OBJ_FLAG) $^
spawn_knife.o: spawn_knife.cpp
	$(CC) $(OBJ_FLAG) $^
spawn_skull.o: spawn_skull.cpp
	$(CC) $(OBJ_FLAG) $^
toggle_fullscreen.o: toggle_fullscreen.cpp
	$(CC) $(OBJ_FLAG) $^
update_generators.o: update_generators.cpp
	$(CC) $(OBJ_FLAG) $^
update_knife_collisions.o: update_knife_collisions.cpp
	$(CC) $(OBJ_FLAG) $^
update_transform_components.o: update_transform_components.cpp
	$(CC) $(OBJ_FLAG) $^
update_animations.o: update_animations.cpp
	$(CC) $(OBJ_FLAG) $^
update_collisions.o: update_collisions.cpp
	$(CC) $(OBJ_FLAG) $^
update_rotations.o: update_rotations.cpp
	$(CC) $(OBJ_FLAG) $^

clean:
	rm -f $(OBJ_FILES) $(BINARY)

