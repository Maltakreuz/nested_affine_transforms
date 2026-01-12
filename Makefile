CXX = g++
CXXFLAGS = -Wall -Wextra -O2
LDFLAGS = `sdl2-config --cflags --libs` -lSDL2_ttf -lSDL2_image -lglew32 -lopengl32

all:
	$(CXX) $(CXXFLAGS) main.cpp $(LDFLAGS) # -o nested_affine_transforms
