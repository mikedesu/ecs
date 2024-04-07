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
					cleanup.o \
					render_debug_panel.o
MAIN_FILE=main.cpp
SDL_FLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
BINARY=game

# main game binary
$(BINARY): $(MAIN_FILE) $(OBJ_FILES)
	g++ $^ $(SDL_FLAGS) -o $@

# object files
distance.o: distance.cpp
fps.o: fps.cpp
frame_time.o: frame_time.cpp
init_rng.o: init_rng.cpp
render_frame.o: render_frame.cpp
render_sprites.o: render_sprites.cpp
spawn_generator.o: spawn_generator.cpp
update_rotations.o: update_rotations.cpp
update_animations.o: update_animations.cpp
render_debug_panel.o: render_debug_panel.cpp
update_transform_components.o: update_transform_components.cpp
update_generators.o: update_generators.cpp
update_knife_collisions.o: update_knife_collisions.cpp
init_debug_texture_rects.o: init_debug_texture_rects.cpp
init_target_texture_rects.o: init_target_texture_rects.cpp
get_next_entity_id.o: get_next_entity_id.cpp
cleanup.o: cleanup.cpp
	g++ -c $^

clean:
	rm -f $(OBJ_FILES) $(BINARY)

