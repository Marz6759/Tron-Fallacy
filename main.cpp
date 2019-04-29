#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include "curses.h"
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <vector>
#include <SFML/Audio.hpp>
#include <string.h>
void suspense();
using namespace std;
unordered_map <int, int> dec_inc = {{1,2},{2,1},{3,4},{4,3}};
sf::Sound sound;
sf::SoundBuffer buffer;
void playsound(string filepath){
    try{
        if (!buffer.loadFromFile(filepath)){
            return;
        }
    }
    catch (...)
    {
    }
    sound.setBuffer(buffer);
    sound.play();
}
class grid{
public:

    int length=0, width=0;
    char grid[1000][1000];
    void print(char* message){
        int x = strlen(message);
        move(length/2, (width*3-x)/2);
        printw("%s\n", message);
        refresh();
    }
    void grid_builder(int x, int y){
        length = x;
        width = y;
        attron(COLOR_PAIR(1));
        attron(A_BOLD);
        print("B U I L D I N G\tG R I D\n");
        sleep(1);
        clear();
        for(int i=0; i<x; i++){
            for (int j=0; j<y; j++){
                if (i==length-1 || i == 0){
                    grid[i][j]='_';
                }
                else if(j== width-1 || j == 0){
                    grid[i][j]='|';
                }
                else{
                    grid[i][j]=' ';
                }
            }
        }
        suspense();
        clear();
        print("Done\n");
        attroff(COLOR_PAIR(1));
        attroff(A_BOLD);
        refresh();
        sleep(1);
        clear();
        refresh();
    }
    void grid_output(){
        clear();
        refresh();
        for(int i=0; i<length; i++){

            for (int j=0; j<width; j++){
                if (grid[i][j] == '.'){
                    printw(" ");
                    attron(COLOR_PAIR(1));
                    printw(".");
                    attroff(COLOR_PAIR(1));
                    printw(" ");
                }
                else if (grid[i][j] == ','){
                    printw(" ");
                    attron(COLOR_PAIR(2));
                    printw(".");
                    attroff(COLOR_PAIR(2));
                    printw(" ");
                } else if(grid[i][j] == '*' || grid[i][j]=='#'){
                    attron(COLOR_PAIR(3));
                    printw(" %c ", grid[i][j]);
                    attroff(COLOR_PAIR(3));
                }
                else {
                    printw(" %c ", grid[i][j]);

                }
            }
            printw("\n");
        }
        refresh();
    }
};

grid grid1;
void suspense(){
    for (int i =0; i < 3; i++){
        move(grid1.length/2+i-1, (grid1.width*3-1)/2);
        printw(".\n");
        refresh();
        sleep(1);
    }
}

void chapter1(){
    grid1.print("Welcome user.");
    clear();
    grid1.print("Enter the grid.");
    clear();
}

void bikesgame(){
    class computer{
    public:
        unordered_map <int, int> freespace;
        int move(int direction, pair <int, int> location){
            int x, y, maxdirec=1;
            vector <int> unblocked;
            for (int direc=1; direc <5; direc++){
                x=location.first, y=location.second;
                if (direc!=dec_inc[direction]){
                    freespace[direc]=0;
                    if (direc==1){
                        x--;
                    }
                    else if (direc==2){
                        x++;
                    }
                    else if (direc == 3){
                        y++;
                    }
                    else if (direc == 4){
                        y--;
                    }
                    while(grid1.grid[x][y] == ' '){
                        if (direc==1){
                            x--;
                        }
                        else if (direc==2){
                            x++;
                        }
                        else if (direc == 3){
                            y++;
                        }
                        else if (direc == 4){
                            y--;
                        }
                        freespace[direc]+=1;
                    }
                }
                else{
                    freespace[dec_inc[direc]]=0;
                    continue;
                }
                if (freespace[direc] > freespace[maxdirec]){
                    maxdirec=direc;
                }
                if (grid1.grid[x][y]=='|' || grid1.grid[x][y]!='_'){
                    unblocked.push_back(direc);
                }
            }
            if (!unblocked.empty()){
                for (int direc: unblocked){
                    if(freespace[direc]>freespace[maxdirec]){
                        maxdirec=direc;
                    }
                    else if (freespace[direc]>2){
                        maxdirec=direc;
                    }
                }
                unblocked.clear();
            }
            switch (maxdirec){
                case 1:
                    return KEY_UP;
                case 2:
                    return KEY_DOWN;
                case 3:
                    return KEY_RIGHT;
                case 4:
                    return KEY_LEFT;
                default:
                    printw("ERROR");
            }
        }
    };
    computer ai;
    class bike{
    public:

        bool alive= true;
        pair <int, int> location;
        int direction=1, speed=0, maxspeed=2;
        string name;
        char mapper, trail;
        int color=0;
        void bike_builder(int x, string y, pair<int, int> xy, char point, char trailer){
            color = x;
            name = y;
            location= xy;
            grid1.grid[xy.first][xy.second]=point;
            mapper = point;
            trail =trailer;
        }
        void direct(int input_direction){
            if(input_direction == direction && speed < maxspeed)
                speed+=1;
            else if(input_direction == direction && speed == maxspeed)
                speed=2;
            else if (dec_inc[direction] == input_direction && speed >0){
                speed-=1;
            }
            else{
                direction = input_direction;
                speed = 1;
            }
            if (speed == 0){
                speed=1;
            }

        }
        void crash(int x, int y){
            playsound("Files/crash.ogg");
            grid1.grid[x][y]='*';
            grid1.grid_output();
            refresh();
            usleep(250000);
            grid1.grid[x][y]='#';
            grid1.grid_output();
            refresh();
            usleep(250000);
            grid1.grid[x][y]=trail;
            grid1.grid_output();
            refresh();
            usleep(250000);
        }
        void move(){
            if (speed == 0){
                grid1.grid_output();
                return;
            }
            int x = location.first, y =location.second;
            for (int steps=0; steps!=speed; steps++){
                int x1, y1;
                if (direction==1){
                    if (grid1.grid[x-1][y]==' '){
                        grid1.grid[x][y] = trail;
                        grid1.grid[x-1][y]=mapper;
                        x-=1;
                        grid1.grid_output();
                        refresh();
                    }
                    else{
                        x1= x-1;
                        y1= y;
                        grid1.grid[x][y] = trail;
                        crash(x1,x=y1);
                        alive=false;
                    }
                }
                else if (direction ==3){
                    if (grid1.grid[x][y+1]==' '){
                        grid1.grid[x][y] = trail;
                        grid1.grid[x][y+1]=mapper;
                        y+=1;
                        grid1.grid_output();
                        refresh();
                    }
                    else{
                        x1= x;
                        y1= y+1;
                        grid1.grid[x][y] = trail;
                        crash(x1,x=y1);
                        alive=false;
                    }
                }
                else if (direction ==2){
                    if (grid1.grid[x+1][y]==' '){
                        grid1.grid[x][y] = trail;
                        grid1.grid[x+1][y]=mapper;
                        x+=1;
                        grid1.grid_output();
                        refresh();
                    }
                    else{
                        x1= x+1;
                        y1= y;
                        grid1.grid[x][y] = trail;
                        crash(x1,x=y1);
                        alive=false;
                    }
                }
                else if (direction ==4){
                    if (grid1.grid[x][y-1]==' '){
                        grid1.grid[x][y] = trail;
                        grid1.grid[x][y-1]=mapper;
                        y-=1;
                        grid1.grid_output();

                        refresh();
                    }
                    else{
                        x1= x;
                        y1= y-1;
                        grid1.grid[x][y] = trail;
                        crash(x1,x=y1);
                        alive=false;
                    }
                }
            }
            location.first=x;
            location.second=y;
        }
    };
    //Todo:- Get terminal size.
    int l, w;
    getmaxyx(stdscr, l, w);
    grid1.grid_builder(l-1,w/3);
    grid1.grid_output();
    bike player1, clu;
    l = grid1.length/2;
    w =grid1.width/2;
    player1.bike_builder(1,"Marz", make_pair(l,w+1), 'P', '.');
    clu.bike_builder(2, "Clu", make_pair(l, w), 'C', ',');
    clu.maxspeed=3;
    int movement, movement2;
    nodelay(stdscr, TRUE);
    noecho();
    playsound("Files/track.ogg");
    while (player1.alive && clu.alive){
        clear();
        if ((movement = getch()) == ERR) {
            player1.move();
            movement2 = ai.move(clu.direction, clu.location);
            switch (movement2){
                case KEY_UP:
                    clu.direct(1);
                    clu.move();
                    break;
                case KEY_LEFT:
                    clu.direct(4);
                    clu.move();
                    break;
                case KEY_DOWN:
                    clu.direct(2);
                    clu.move();
                    break;
                case KEY_RIGHT:
                    clu.direct(3);
                    clu.move();
                    break;
                default:
                    continue;
            }
            usleep(500000);
        }

        else{
            switch (movement){
                case 119:
                    player1.direct(1);
                    player1.move();
                    break;
                case 97:
                    player1.direct(4);
                    player1.move();
                    break;
                case 115:
                    player1.direct(2);
                    player1.move();
                    break;
                case 100:
                    player1.direct(3);
                    player1.move();
                    break;
                default:
                    continue;
            }
            movement2 = ai.move(clu.direction, clu.location);
            switch (movement2){
                case KEY_UP:
                    clu.direct(1);
                    clu.move();
                    break;
                case KEY_LEFT:
                    clu.direct(4);
                    clu.move();
                    break;
                case KEY_DOWN:
                    clu.direct(2);
                    clu.move();
                    break;
                case KEY_RIGHT:
                    clu.direct(3);
                    clu.move();
                    break;
                default:
                    continue;
            }
            usleep(500000);
        }
    }
    if (!player1.alive){
        clear();
        grid1.print("GAME OVER");
        playsound("Files/endofline.ogg");
        refresh();
        sleep(5);
    } else{
        clear();
        grid1.print("YOU WIN");
        refresh();
        sleep(2);
    }
}


int main() {
    initscr();
    start_color();
    playsound("Files/battle.ogg");

    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3,COLOR_YELLOW, COLOR_RED);
    //chapter1();
    bikesgame();
    endwin();
    return 0;
}