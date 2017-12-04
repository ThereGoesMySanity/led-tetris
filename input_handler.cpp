#include "input_handler.h"
NCursesHandler::NCursesHandler(){
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    nonl();
    keypad(stdscr, TRUE);
    timeout(100);
    refresh();
}

int NCursesHandler::getKey(){
    int c = getch();
    switch (c) {
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_DOWN:
        case KEY_UP:
        case 'z':
        case 'x':
        case 'c':
            return c;
    }
}
LinuxInputHandler::LinuxInputHandler(){
    file = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    for(int j : INPUTS){
        keyState[j] = false;
    }
}
void LinuxInputHandler::keyOff() {
    for(int i : INPUTS){
        keyState[i] = false;
    }
}
int LinuxInputHandler::getKey(){
    struct input_event e;
    ssize_t bytes = read(file, &e, sizeof(struct input_event));
    while(bytes > 0 ){
        if(e.value == 1 && e.type == EV_KEY){
            switch (e.code) {
                case KEY_LEFT:
                case KEY_RIGHT:
                    move(e.code == KEY_LEFT);
                case KEY_DOWN:
                case KEY_UP:
                case KEY_Z:
                case KEY_X:
                case KEY_C:
                    printf("Key pressed: %i\n", e.code);
                    keyState[(int)e.code] = true;
                    return e.code;
            }
        }
        bytes = read(file, &e, sizeof(struct input_event));
    }
    return 0;
}
