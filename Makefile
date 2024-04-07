all: game

OBJ_FILES=fps.o \
					distance.o \
					frame_time.o \
					init_rng.o \
					render_sprites.o \
					spawn_generator.o \
					render_frame.o \
					update_rotations.o \
					update_animations.o \
					update_transform_components.o \
					update_knife_collisions.o \
					update_generators.o \
					init_debug_texture_rects.o \
					init_target_texture_rects.o \
					get_next_entity_id.o \
					handle_input_component.o \
					cleanup.o \
					spawn_skull.o \
					load_knife_sheet_texture.o \
					load_eyeball_sheet_texture.o \
					load_skull_sheet_texture.o \
					init_target_texture.o \
					init_gfont.o \
					init_ttf.o \
					init_img.o \
					create_renderer.o \
					create_window.o \
					handle_input.o \
					spawn_knife.o \
					render_debug_panel.o
MAIN_FILE=main.cpp
SDL_FLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
BINARY=game

# main game binary
$(BINARY): $(MAIN_FILE) $(OBJ_FILES)
	g++ $^ $(SDL_FLAGS) -o $@

# object files
cleanup.o: cleanup.cpp
distance.o: distance.cpp
fps.o: fps.cpp
frame_time.o: frame_time.cpp
get_next_entity_id.o: get_next_entity_id.cpp
handle_input_component.o: handle_input_component.cpp
init_debug_texture_rects.o: init_debug_texture_rects.cpp
init_target_texture_rects.o: init_target_texture_rects.cpp
init_rng.o: init_rng.cpp
render_frame.o: render_frame.cpp
render_sprites.o: render_sprites.cpp
render_debug_panel.o: render_debug_panel.cpp
spawn_generator.o: spawn_generator.cpp
update_generators.o: update_generators.cpp
update_knife_collisions.o: update_knife_collisions.cpp
update_transform_components.o: update_transform_components.cpp
update_rotations.o: update_rotations.cpp
update_animations.o: update_animations.cpp
spawn_skull.o: spawn_skull.cpp
load_knife_sheet_texture.o: load_knife_sheet_texture.cpp
load_eyeball_sheet_texture.o: load_eyeball_sheet_texture.cpp
load_skull_sheet_texture.o: load_skull_sheet_texture.cpp
init_target_texture.o: init_target_texture.cpp
init_gfont.o: init_gfont.cpp
init_ttf.o: init_ttf.cpp
init_img.o: init_img.cpp
create_renderer.o: create_renderer.cpp
create_window.o: create_window.cpp
handle_input.o: handle_input.cpp
spawn_knife.o: spawn_knife.cpp
	g++ -c $^

clean:
	rm -f $(OBJ_FILES) $(BINARY)

