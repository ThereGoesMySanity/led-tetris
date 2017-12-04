#include <curses.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <map>
#include <algorithm>
#include <cstring>
#include "input_handler.h"
#include "rpi-rgb-led-matrix/include/led-matrix.h"
#include "rpi-rgb-led-matrix/include/graphics.h"
#include "rpi-rgb-led-matrix/include/canvas.h"
#include "rpi-rgb-led-matrix/include/threaded-canvas-manipulator.h"

using namespace rgb_matrix;
struct block {
    int x, y;
    bool space[4][4];
    struct Color c;
};
struct block NULL_BLOCK = { 0, 0,
    {
        {0,0,0,0},
        {0,0,0,0},
        {0,0,0,0},
        {0,0,0,0}
    }, {0, 0, 0} };

const struct block I = { 3, 0,
    {
        {0,0,0,0},
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0}
    }, {0, 255, 255} };
const struct block J = { 2, 0,
    {
        {0,0,0,0},
        {0,1,0,0},
        {0,1,1,1},
        {0,0,0,0}
    }, {0, 0, 255} };
const struct block L = { 3, 0,
    {
        {0,0,0,0},
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0}
    }, {255, 100, 0} };
const struct block T = { 3, 0,
    {
        {0,0,0,0},
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0}
    }, {255, 0, 255} };
const struct block O = { 3, 0,
    {
        {0,0,0,0},
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0}
    }, {255, 255, 0} };
const struct block S = { 3, 0,
    {
        {0,0,0,0},
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0}
    }, {0, 255, 0} };
const struct block Z = { 3, 0,
    {
        {0,0,0,0},
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0}
    }, {255, 0, 0} };

const struct block blocks[] = {I,J,L,T,O,S,Z};

const struct Color GREY(50,50,50);
struct Pixel {
    Pixel() : r(0), b(0), g(0) {}
    Pixel(Color c) : r(c.r), b(c.b), g(c.g) {}
    uint8_t r, g, b;
};
class Tetris : public ThreadedCanvasManipulator {
    public:
        Tetris(RGBMatrix *mat, InputHandler *h) : ThreadedCanvasManipulator(mat), handler(h) {
            for(int i = 0; i < 32; i++) {
                grid[i] = (bool*)calloc(10, sizeof(bool));
                colorGrid[i] = (Pixel*)calloc(10, sizeof(Pixel));
            }
            for(int i = 0; i < 4; i++){
                nextBlock[i] = blocks[rand()%7];
            }
        }
        virtual void Run();
        int update();
        void draw(Canvas* canvas);
        bool checkBlock(struct block b);
        void addBlock(struct block b);
        void rotateBlock(struct block *b, bool clockwise);

        std::map<int, bool> input;
        std::map<int, int> delay;
    private:
        int left = 0, right = 0;
        InputHandler *handler;
        bool *grid [32];
        struct Pixel *colorGrid[32];
        struct block currentBlock = blocks[rand()%7];
        struct block *nextBlock = (struct block*)calloc(4, sizeof(struct block));
        struct block *heldBlock = &NULL_BLOCK;
        bool swapped;
        int speed = 150;
};
