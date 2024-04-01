all: game

game: main.cpp
	g++ main.cpp -lSDL2 -lSDL2_image -o game

clean:
	rm -f game


