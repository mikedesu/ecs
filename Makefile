all: game

CC=g++
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
	$(CC) $^ $(SDL_FLAGS) -o $@

# object files
cleanup.o: cleanup.cpp
create_renderer.o: create_renderer.cpp
create_window.o: create_window.cpp
distance.o: distance.cpp
fps.o: fps.cpp
frame_time.o: frame_time.cpp
generator_set_active.o: generator_set_active.cpp
get_next_entity_id.o: get_next_entity_id.cpp
get_num_enemies_killed.o: get_num_enemies_killed.cpp
handle_input_component.o: handle_input_component.cpp
handle_input.o: handle_input.cpp
init_debug_texture_rects.o: init_debug_texture_rects.cpp
init_rng.o: init_rng.cpp
init_target_texture.o: init_target_texture.cpp
init_target_texture_rects.o: init_target_texture_rects.cpp
init_gfont.o: init_gfont.cpp
init_img.o: init_img.cpp
init_ttf.o: init_ttf.cpp
load_coin_sheet_texture.o: load_coin_sheet_texture.cpp
load_debug_text.o: load_debug_text.cpp
load_eyeball_sheet_texture.o: load_eyeball_sheet_texture.cpp
load_knife_sheet_texture.o: load_knife_sheet_texture.cpp
load_skull_sheet_texture.o: load_skull_sheet_texture.cpp
render_frame.o: render_frame.cpp
render_sprites.o: render_sprites.cpp
render_debug_panel.o: render_debug_panel.cpp
spawn_coin.o: spawn_coin.cpp
spawn_eyeball.o: spawn_eyeball.cpp
spawn_generator.o: spawn_generator.cpp
spawn_knife.o: spawn_knife.cpp
spawn_skull.o: spawn_skull.cpp
toggle_fullscreen.o: toggle_fullscreen.cpp
update_generators.o: update_generators.cpp
update_knife_collisions.o: update_knife_collisions.cpp
update_transform_components.o: update_transform_components.cpp
update_animations.o: update_animations.cpp
update_collisions.o: update_collisions.cpp
update_rotations.o: update_rotations.cpp
	$(CC) $(OBJ_FLAG) $^

clean:
	rm -f $(OBJ_FILES) $(BINARY)

