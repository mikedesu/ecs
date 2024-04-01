all: game

game: main.cpp
	g++ main.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -o game

clean:
	rm -f game


