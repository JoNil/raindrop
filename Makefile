all:
	g++ -std=c++14 -Wall raindrop.cpp ./common/*.cpp -o raindrop -lGLEW -lGL -lglfw

clean:
	rm -f raindrop

.PHONY: all clean