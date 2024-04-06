all: game

OBJ_FILES=fps.o \
					distance.o \
					frame_time.o
MAIN_FILE=main.cpp
SDL_FLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
BINARY=game

# main game binary
$(BINARY): $(MAIN_FILE) $(OBJ_FILES)
	g++ $^ $(SDL_FLAGS) -o $@

# object files
frame_time.o: frame_time.cpp
fps.o: fps.cpp
distance.o: distance.cpp
	g++ -c $^

clean:
	rm -f $(OBJ_FILES) $(BINARY)



