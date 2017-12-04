CXXFLAGS=-std=c++11
OBJECTS=tetris.cpp input_handler.cpp
LDFLAGS+=-lncurses

RGB_LIB_DISTRIBUTION=rpi-rgb-led-matrix
RGB_INCDIR=$(RGB_LIB_DISTRIBUTION)/include
RGB_LIBDIR=$(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

tetris: $(OBJECTS) $(RGB_LIBRARY)
	g++ $(CXXFLAGS) $(OBJECTS) -o led_tetris $(LDFLAGS)
