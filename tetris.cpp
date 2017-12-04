#include "tetris.h"
volatile bool interrupt = false;
static void InterruptHandler(int signo) {
    interrupt = true;
}
//returns false if the block cannot be placed in this position
bool Tetris::checkBlock(struct block b){
    if(b.y > 28){
        for(int i = 0; i < 4; i++){
            if(b.space[32-b.y][i]){
                return false;
            }
        }
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(b.space[i][j] && grid[b.y+i][b.x+j]){
                return false;
            }
        }
    }
    return true;
}
int Tetris::update() {
    std::map<int, bool> input;
    if(handler->canGetKeyState()){
        input = handler->keyState;
    } else {
        input = this->input;
    }
    for(int i : INPUTS){
        if(delay[i] > 0){
            delay[i]--;
        }
        if(!handler->keyState[i])delay[i] = 0;

    }
    if(delay[KEY_LEFT] == 0 && input[KEY_LEFT]) {
        currentBlock.x -= left;
        for(int x = 0; !checkBlock(currentBlock) && x < left; x++){
            currentBlock.x++;
        }
        if(currentBlock.x < 0){
            for(int x = 0; x < left; x++){
                for(int i = 0; i < 4; i++){
                    if(currentBlock.space[i][-currentBlock.x-1]){
                        currentBlock.x++;
                        break;
                    }
                }
            }
        }
        delay[KEY_LEFT] = 2;
        if(input == this->input)input[KEY_LEFT] = false;
    }
    left = 0;
    if(delay[KEY_RIGHT] == 0 && input[KEY_RIGHT]) {
        currentBlock.x += right;
        for(int x = 0; !checkBlock(currentBlock) && x < right; x++){
            currentBlock.x--;
        }
        if(currentBlock.x+4 > 10){
            for(int x = 0; x < right; x++){
                for(int i = 0; i < 4; i++){
                    if(currentBlock.space[i][-currentBlock.x+10]){
                        currentBlock.x--;
                        break;
                    }
                }
            }
        }
        delay[KEY_RIGHT] = 2;
        if(input == this->input)input[KEY_RIGHT] = false;
    }
    right = 0;
    if(delay[KEY_DOWN] == 0 && input[KEY_DOWN]) {
        //speed up
        currentBlock.y++;
        if(!checkBlock(currentBlock)){
            currentBlock.y--;
        }
        if(input == this->input)input[KEY_DOWN] = false;
    }
    if(delay[KEY_X] == 0 && input[KEY_X]) {
        //rotate clockwise
        rotateBlock(&currentBlock, true);
        if(!checkBlock(currentBlock)){
            currentBlock.y--;
        }
        if(!checkBlock(currentBlock)){
            rotateBlock(&currentBlock, false);
            currentBlock.y++;
        }
        delay[KEY_X] = 9;
        if(input == this->input)input['x'] = false;
    }
    if(delay[KEY_Z] == 0 && input[KEY_Z]) {
        //rotate counterclockwise
        rotateBlock(&currentBlock, false);
        if(!checkBlock(currentBlock)){
            currentBlock.y--;
        }
        if(!checkBlock(currentBlock)){
            rotateBlock(&currentBlock, true);
            currentBlock.y++;
        }
        delay[KEY_Z] = 9;
        if(input == this->input)input['z'] = false;
    }
    if(delay[KEY_UP] == 0 && input[KEY_UP]) {
        //drop
        while(checkBlock(currentBlock)){
            currentBlock.y++;
        }
        currentBlock.y--;
        delay[KEY_UP] = 9;
        input[KEY_UP] = false;
    }
    if(delay[KEY_C] == 0 && input[KEY_C]) {
        //swap
        if(!swapped){
            currentBlock.y = 0;
            currentBlock.x = 3;
            struct block *temp = (struct block*)malloc(sizeof(currentBlock));
            std::memcpy(temp, &currentBlock, sizeof(currentBlock));
            if(heldBlock != &NULL_BLOCK){
                currentBlock = *heldBlock;
            } else {
                currentBlock = blocks[rand()%7];
            }
            heldBlock = temp;
            //TODO: maybe a memory leak
            swapped = true;
        }
        delay[KEY_C] = 2;
        if(input == this->input)input['c'] = false;
    }
    handler->keyOff();
    currentBlock.y++;
    if(!checkBlock(currentBlock)){
        currentBlock.y--;
        addBlock(currentBlock);
        if(!checkBlock(currentBlock)){
            return 1;
        }
    }
    int count = 0;
    for(int i = 0; i < 32; i++){
        bool b = true;
        for(int j = 0; j < 10; j++){
            b &= grid[i][j];
        }
        if(b){
            count++;
            for(int j = 0; j < 10; j++){
                colorGrid[i][j] = Pixel();
                grid[i][j] = 0;
            }
            bool *temp = grid[i];
            Pixel *temp2 = colorGrid[i];
            for(int j = i; j > 0; j--) {
                colorGrid[j] = colorGrid[j-1];
                grid[j] = grid[j-1];
            }
            grid[0] = temp;
            colorGrid[0] = temp2;
        }
    }
    speed -= count;
    if(count == 4){
        //TODO: it's a tetris
    }
    return 0;
}
void Tetris::rotateBlock(struct block *b, bool clockwise){
    bool newArr[4][4];
    bool high = false;
    bool low = false;
    for(int j = 0; j < 4; j++) {
        for(int i = 0; i < 4; i++) {
            newArr[i][j] = b->space[clockwise? 3-j : j][clockwise? i : 3-i];
            high |= newArr[i][j] && (b->x + j > 9);
            low |= newArr[i][j] && (b->x + j < 0);
        }
    }
    std::copy(&newArr[0][0], &newArr[0][0]+16, &b->space[0][0]);
    if(high){
        b->x--;
        for(int i = 1; i < 3; i++){
            if(b->space[i][4])b->x--;
        }
    }
    if(low){
        b->x++;
        for(int i = 1; i < 3; i++){
            if(b->space[i][0])b->x++;
        }
    }
}
void Tetris::addBlock(struct block b){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(b.space[j][i]){
                grid[b.y+j][b.x+i] = true;
                colorGrid[b.y+j][b.x+i] = Pixel(b.c);
            }
        }
    }
    currentBlock = nextBlock[0];
    for(int i = 0; i < 3; i++){
        nextBlock[i] = nextBlock[i+1];
    }
    nextBlock[3] = blocks[rand()%7];

    swapped = false;
}
void Tetris::Run(){
    handler->setMove(&left, &right);
    printf("Running\n");
    while(running() && !interrupt){
        long int i = clock()/1000;
        int x = update();
        if(x == 1){
            InterruptHandler(1);
        }
        draw(canvas());
        i = clock()/1000-i;
        usleep(std::max(speed-i, 0l)*1000);
    }
}
void Tetris::draw(Canvas *canvas){
    canvas->Fill(0,0,0);
    uint8_t r, g, b;

    for(int i = 0; i < 6; i++){
        //TODO: fancy colors on tetris
        DrawLine(canvas, 0, i, 64, i, GREY);
        DrawLine(canvas, 0, 31-i, 64, 31-i, GREY);
    }
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 32; j++){
            if(grid[j][i]){
                r = colorGrid[j][i].r;
                g = colorGrid[j][i].g;
                b = colorGrid[j][i].b;
                canvas->SetPixel(j*2, 25-i*2, r, g, b);
                canvas->SetPixel(j*2+1, 25-i*2, r, g, b);
                canvas->SetPixel(j*2, 24-i*2, r, g, b);
                canvas->SetPixel(j*2+1, 24-i*2, r, g, b);
            }
        }
    }
    r = currentBlock.c.r;
    g = currentBlock.c.g;
    b = currentBlock.c.b;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(currentBlock.space[j][i]){
                canvas->SetPixel((currentBlock.y+j)*2, 25-(currentBlock.x+i)*2, r, g, b);
                canvas->SetPixel((currentBlock.y+j)*2+1, 25-(currentBlock.x+i)*2, r, g, b);
                canvas->SetPixel((currentBlock.y+j)*2, 24-(currentBlock.x+i)*2, r, g, b);
                canvas->SetPixel((currentBlock.y+j)*2+1, 24-(currentBlock.x+i)*2, r, g, b);
            }
        }
    }
    if(heldBlock != &NULL_BLOCK){
        r = heldBlock->c.r;
        g = heldBlock->c.g;
        b = heldBlock->c.b;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                if(heldBlock->space[j][i]){
                    canvas->SetPixel(j, 30-i, r, g, b);
                }
            }
        }
    }
    for(int bl = 0; bl < 4; bl++){
        r = nextBlock[bl].c.r;
        g = nextBlock[bl].c.g;
        b = nextBlock[bl].c.b;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                if(nextBlock[bl].space[j][i]){
                    canvas->SetPixel(j+4*bl, 4-i, r, g, b);
                }
            }
        }
    }
}



int main(int argc, char** argv){
    int ch;

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);
    RGBMatrix::Options defaults;
    defaults.chain_length = 2;
    rgb_matrix::RuntimeOptions runtime_opt;
    RGBMatrix *mat = CreateMatrixFromFlags(&argc, &argv, &defaults, &runtime_opt);
    InputHandler *h = new LinuxInputHandler();
    Tetris *tet = new Tetris(mat, h);
    tet->Start();
    int c;
    while(!interrupt){
        h->update();
        if(!h->canGetKeyState()){
            c = h->getKey();
            tet->input[c] = true;
        }
        usleep(10*1000);
    }
    delete h;
    delete mat;
    delete tet;
    endwin();
    refresh();
}
