all: game

OBJ_FILES=fps.o \
					distance.o \
					frame_time.o \
					init_rng.o \
					render_sprites.o \
					spawn_generator.o \
					render_frame.o \
					update_rotations.o
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
	g++ -c $^

clean:
	rm -f $(OBJ_FILES) $(BINARY)

