#include <curses.h>
#include <locale.h>
#include <linux/input.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <map>
#ifndef _INPUT_HANDLER_H
#define _INPUT_HANDLER_H
const int INPUTS[] = {KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP, KEY_Z, KEY_X, KEY_C};
class InputHandler {
    public:
        InputHandler(){}
        virtual int getKey() = 0;
        virtual bool canGetKeyState() = 0;
        virtual void update() = 0;
        virtual void keyOff() = 0;
        std::map<int, bool> keyState;
        virtual void setMove(int* left, int* right) = 0;
};
class NCursesHandler : public InputHandler {
    public:
        NCursesHandler() ;
        int getKey();
        bool canGetKeyState(){return false;}
        void update() {}
        void keyOff() {}
};
class LinuxInputHandler : public InputHandler {
    public:
        LinuxInputHandler();
        int getKey();
        bool canGetKeyState() {return true;}
        void update() {
            getKey();
        }

        void setMove(int* l, int* r){
            left = l;
            right = r;
        }
        void keyOff();
    private:
        void move(bool l){
            if(l && left){
                (*left)++;
                printf("%i\n", *left);
            } else if(right){
                (*right)++;
                printf("%i\n", *right);
            }
        }
        int file;
        int* left = nullptr, *right = nullptr;
};
#endif
