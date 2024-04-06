all: game

OBJ_FILES=fps.o
MAIN_FILE=main.cpp
SDL_FLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
BINARY=game

# main game binary
$(BINARY): $(MAIN_FILE) $(OBJ_FILES)
	g++ $^ $(SDL_FLAGS) -o $@

# object files
fps.o: fps.cpp
	g++ -S $^
	g++ -c $^

clean:
	rm -f $(OBJ_FILES) $(BINARY)



