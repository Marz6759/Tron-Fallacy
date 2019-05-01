#include <iostream>
#include "ascii.hxx"
#include "metric.hxx"
#include "xterm.h"
#include "image.hxx"
#include <unistd.h>
#include <unordered_map>
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <vector>
#include <SFML/Audio.hpp>
#include <tuple>
#include <cstdint>
#include <cstdlib>
#include <cstring>

void suspense();

using namespace std;

unordered_map <int, int> dec_inc = {{1,2},{2,1},{3,4},{4,3}};
int cols, lines;
bool aiplay=false;

sf::Sound sound, effect;

sf::SoundBuffer buffer, buffer2;

void playsound(string filepath, int option=2){
    if (option==1){
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
    else{
        try{
            if (!buffer2.loadFromFile(filepath)){
                return;
            }
        }
        catch (...)
        {
        }
        effect.setBuffer(buffer2);
        effect.play();
    }
}

WINDOW* InitCurses(){
    putenv("TERM=xterm-256color");
    getmaxyx(stdscr, lines, cols);
    nodelay(stdscr, TRUE);
    timeout(0);
    start_color();
    for (int i = 0; i < COLORS; i++)
        init_pair(i, i, 0);
    return stdscr;
}

class grid{
public:

    int length=0, width=0;
    char grid[1000][1000];
    void print(char* message, int get=2){
        //3 for immediate print, 2 for default slow print and 4 for overwrite on screen print.
        nodelay(stdscr, TRUE);
        noecho();
        int speed = 90000;
        if(get<4){
            clear();
        }

        string string1=message;
        size_t x = strlen(message), counter=0;
        move(length/2, (width*3-x)/2);
        if (get == 3){
            printw("%s\n", message);
            refresh();
            sleep(1);
        }
        else if (get==5){
            printw("%s\n", message);
            refresh();
            sleep(1);
        }
        else {
            while (counter != x) {
                int ch = getch();
                if (ch == 10) {
                    speed = 10000;
                }
                printw("%c", string1[counter]);
                refresh();
                usleep(speed);
                counter++;

            }
            printw("\n");
            if (get != 1) {
                getchar();
            }
        }
    }

    void grid_builder(int x, int y){
        length = x;
        width = y;
        attron(COLOR_PAIR(6));
        attron(A_BOLD);
        playsound("Files/battle.ogg");
        print("B U I L D I N G\tG R I D\n",3);
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
        print("Done\n",3);
        attroff(COLOR_PAIR(6));
        attroff(A_BOLD);
        refresh();
        sleep(1);
        clear();
        refresh();
    }
    void grid_output(){
        clear();
        for(int i=0; i<length; i++){

            for (int j=0; j<width; j++){
                if (grid[i][j] == '.'){
                    printw(" ");
                    attron(COLOR_PAIR(6));
                    printw(".");
                    attroff(COLOR_PAIR(6));
                    printw(" ");
                }
                else if (grid[i][j] == ','){
                    printw(" ");
                    attron(COLOR_PAIR(1));
                    printw(".");
                    attroff(COLOR_PAIR(1));
                    printw(" ");
                } else if(grid[i][j] == '*' || grid[i][j]=='#'){
                    attron(COLOR_PAIR(99));
                    printw(" %c ", grid[i][j]);
                    attroff(COLOR_PAIR(99));
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

const int STDOUT(1);

void displayimg(char *argv, bool loop= true){
    ImageLoader *loader = ImageLoader::get_instance(argv);
    vector<vector<vector<uint32_t>>> img;;
    vector<uint32_t> delay;

    tie(delay, img) = loader->load(argv, cols/2, 1 << 16);

    if (!isatty(STDOUT)) {
        vector<vector<string>> asc = IMG2ASCII(img).convert();
        uint32_t fptr = 0;
        printf("export TERM=xterm-256color\n");
        printf("clear\n");
        for (auto& frm: asc) {
            uint32_t rptr = 1;
            for (auto& row: frm) {
                printf("echo \"[%d;1H\"\n", rptr);
                printf("echo \"%s\"\n", row.c_str());
                ++rptr;
            }
            printf("sleep %.2f\n", delay[fptr] / 100.);
            ++fptr;
        }
        clear();
        refresh();
        return;
    }

    refresh();

    vector<vector<string>> asc = IMG2ASCII(img).convert_raw();
    int zoom_ptr = 0;
    for (;;) {
        uint32_t fptr = 0;
        for (auto& frm: asc) {
            int rptr = 0, cptr = 0;
            for (int r = zoom_ptr; r < frm.size(); r++) {
                auto& row = frm[r];
                for (cptr = 0; cptr < row.size(); cptr++) {
                    int col = (unsigned char)row[cptr];
                    attrset(COLOR_PAIR(col));
                    mvprintw(rptr, cptr*2, "##");
                }
                ++rptr;
            }
            attrset(COLOR_PAIR(0));
            refresh();
            usleep(10000 * delay[fptr]);
            ++fptr;
        }
        if (loop == true){
            int ch = getch();
            if (ch == 10) {
                break;
            }
        } else{
            getchar();
            break;
        }
    }

}

void suspense(){
    clear();
    nodelay(stdscr, TRUE);
    noecho();
    int ch = getch(), speed=1000000;
    for (int i =0; i < 3; i++){
        ch = getch();
        if (ch == 10) {
            speed = 100000;
        }
        move(grid1.length/2+i-1, (grid1.width*3-1)/2);
        printw(".\n");
        refresh();
        usleep(speed);
    }
    clear();
    refresh();
}

void bikesgame(){
    initscr();
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
            }
            return maxdirec;
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
            playsound("Files/crash.ogg",1);
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
            trail =' ';
            mapper= ' ';
        }
        void move(){
            if (speed == 0){
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
    grid1.grid_output();
    bike player1, clu, ai1, ai2;
    player1.bike_builder(1,"Marz", make_pair(grid1.length/2,grid1.width/2+1), 'P', '.');
    clu.bike_builder(2, "Clu", make_pair(grid1.length/2, grid1.width/2-5), 'C', ',');
    int movement;
    playsound("Files/track.ogg");

    if (aiplay){
        ai1.bike_builder(2, "ai1", make_pair(grid1.length/2, 1), 'A', ',');
        ai2.bike_builder(2, "ai1", make_pair(grid1.length/2+4, 1), 'A', ',');

        while (player1.alive && clu.alive){
            if ((movement = getch()) == ERR) {
                clu.direct(ai.move(clu.direction, clu.location));
                ai1.direct(ai.move(ai1.direction, ai1.location));
                ai2.direct(ai.move(ai2.direction, ai2.location));
                player1.move();
                clu.move();
                if(ai1.alive)
                    ai1.move();
                if(ai2.alive)
                    ai2.move();
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
                clu.direct(ai.move(clu.direction, clu.location));
                ai1.direct(ai.move(ai1.direction, ai1.location));
                ai2.direct(ai.move(ai2.direction, ai2.location));
                clu.move();
                if(ai1.alive)
                    ai1.move();
                if(ai2.alive)
                    ai2.move();
            }
            grid1.grid_output();
            usleep(250000);
            refresh();
        }
    }

    else{//Two players, WASD for P and IJKL for Clu.
        while (player1.alive && clu.alive){
            if ((movement = getch()) == ERR) {
                player1.move();
                clu.move();
            }

            else{
                switch (movement){
                    case 119:
                        player1.direct(1);
                        player1.move();
                        clu.move();
                        break;
                    case 97:
                        player1.direct(4);
                        player1.move();
                        clu.move();
                        break;
                    case 115:
                        player1.direct(2);
                        player1.move();
                        clu.move();
                        break;
                    case 100:
                        player1.direct(3);
                        player1.move();
                        clu.move();
                        break;
                    case KEY_UP:
                        clu.direct(1);
                        clu.move();
                        player1.move();
                        break;
                    case KEY_LEFT:
                        clu.direct(4);
                        clu.move();
                        player1.move();
                        break;
                    case KEY_DOWN:
                        clu.direct(2);
                        clu.move();
                        player1.move();
                        break;
                    case KEY_RIGHT:
                        clu.direct(3);
                        clu.move();
                        player1.move();
                        break;
                    default:
                        continue;
                }
            }
            grid1.grid_output();
            usleep(250000);
            refresh();
        }
    }


    if (!player1.alive){
        playsound("Files/endofline.ogg",1);
        displayimg("Files/clu_intro.jpg");
        grid1.print("GAME OVER",1);
        refresh();
        sleep(5);
    }
    else{
        clear();
        grid1.print("YOU WIN",1);
        refresh();
        sleep(2);
    }

}

void chapter1(){
    clear();
    attron(COLOR_PAIR(6));
    playsound("Files/recogtrack.ogg",1);
    grid1.print("'Where am I?'");
    getchar();
    suspense();
    attroff(COLOR_PAIR(6));
    grid1.print("Darkness. Everything around you is pitch black.");
    grid1.print("Confused and scared, you start running.");
    grid1.print("To which direction, to what destination? Honest to God, the world may never know.");
    grid1.print("You pass streets");
    grid1.print("roads");
    grid1.print("buildings");
    attron(A_BOLD);
    grid1.print("ALL");
    attron(COLOR_PAIR(6));
    grid1.print("ILLUMINATED",1);
    attron(A_BLINK);
    grid1.print("ILLUMINATED",3);
    attroff(A_BLINK);
    attroff(COLOR_PAIR(6));
    attroff(A_BOLD);
    getchar();
    clear();
    displayimg("Files/street.jpg");

    attron(COLOR_PAIR(6));
    grid1.print("'This isn't happening...'",4);
    attroff(COLOR_PAIR(6));
    grid1.print("You look around you. Not a soul in sight.");
    grid1.print("Just the illuminated streets, buildings and signs.");
    grid1.print("It was just you and the lifeless and quiet city of illumi-",1);
    playsound("Files/landing.ogg");
    displayimg("Files/rec.jpg");
    attron(COLOR_PAIR(6));
    attron(A_BOLD);
    grid1.print("'Oh man this is happening!'",4);
    attroff(COLOR_PAIR(6));
    attroff(A_BOLD);
    grid1.print("You try to run away from the ship,");
    grid1.print("but the ground you were standing on is now a platform rising up to the skies;");
    grid1.print("taking you into the ship.");
    clear();
    displayimg("Files/recog.jpg");
    grid1.print("You find yourself standing in a line with other people.");
    suspense();
    attron(A_ITALIC);
    grid1.print("Things.");
    attroff(A_ITALIC);
    grid1.print("Something about them didn't seem quite human.");
    grid1.print("You look up, and see a number of men in illuminated suits.");
    displayimg("Files/rectify.png");
    grid1.print("One of them approaches the line-up and starts scanning the first in line to your left.",4);
    displayimg("Files/rectify.png");
    attron(COLOR_PAIR(1));
    attron(A_BOLD);
    playsound("Files/rectify.ogg");
    grid1.print("'RECTIFY'",4);
    attroff(COLOR_PAIR(1));
    attroff(A_BOLD);
    grid1.print("You desperately try to move, only to find that your feet is locked and immobilized.");
    displayimg("Files/rectify.png");
    attron(COLOR_PAIR(1));
    attron(A_BOLD);
    playsound("Files/rectify.ogg");
    grid1.print("'RECTIFY'",4);
    attroff(COLOR_PAIR(1));
    attroff(A_BOLD);
    playsound("Files/games.ogg");
    grid1.print("It seems like he is examining the faces of the others standing beside you. And categorizing them.");
    grid1.print("You've been dreading it, but it seems that your turn will soon come up.");
    move(grid1.length/2,0);
    attron(COLOR_PAIR(COLOR_GREEN));
    for(int i = 0 ; i< 4; i++,sleep(1),refresh())
        printw("\t'Not the games'\n\n");
    clear();
    attroff(COLOR_PAIR(COLOR_GREEN));
    grid1.print("The guy besides you starts pleading as his turn comes up.");
    clear();
    move(grid1.length/2,0);
    attron(COLOR_PAIR(COLOR_GREEN));
    printw("\tPLEASE NOT THE GAMES!");
    refresh();
    getchar();
    attroff(COLOR_PAIR(COLOR_GREEN));
    attron(COLOR_PAIR(1));
    attron(A_BOLD);
    playsound("Files/games.ogg");
    grid1.print("'GAMES'",4);
    attroff(COLOR_PAIR(1));
    attroff(A_BOLD);
    move(grid1.length/2,0);
    attron(COLOR_PAIR(COLOR_GREEN));
    printw("\tNO PLEASE!");
    getchar();
    attroff(COLOR_PAIR(COLOR_GREEN));
    grid1.print("The poor man starts flailing as they lead him away.");
    grid1.print("He breaks free and immediately runs towards the ship's exit which lead to a long way down.");
    grid1.print("Dead end...");
    suspense();
    grid1.print("And yet he still jumped. To his death.");
    grid1.print("Were the games all that bad? It was your turn now, maybe you'll get 'rectified'.");
    grid1.print("Whatever that was...");
    attron(COLOR_PAIR(1));
    attron(A_BOLD);
    playsound("Files/games.ogg");
    grid1.print("'GAMES'",4);
    attroff(COLOR_PAIR(1));
    attroff(A_BOLD);
    grid1.print("Dread fills your belly and you resist their cold grip");
    attron(COLOR_PAIR(99));
    grid1.print("CRACK",3);
    attroff(COLOR_PAIR(99));
    grid1.print("Something hits the back of your head and takes out the lights in your head.");
    grid1.print("Looks like they weren't going to risk another suicidal maniac.");
}

void entertron(){
    mvprintw(grid1.length, 0,"Press Enter to continue...");
    playsound("Files/thegrid.ogg");
    displayimg("Files/riding.gif");
    attron(A_BOLD);
    grid1.print("Directed and Coded by");
    attron(COLOR_PAIR(COLOR_CYAN));
    grid1.print("Marzouq Abedur Rahman");
    attroff(COLOR_PAIR(COLOR_CYAN));
    attroff(A_BOLD);
    grid1.print("1823917");
    mvprintw(grid1.length, 0,"Press Enter to continue...");
    displayimg("Files/riding.gif");
    attron(A_BOLD);
    grid1.print("Storyline by");
    attron(COLOR_PAIR(COLOR_YELLOW));
    grid1.print("Ayunni Jasmine");
    attroff(COLOR_PAIR(COLOR_YELLOW));
    attroff(A_BOLD);
    grid1.print("1823917");
    mvprintw(grid1.length, 0,"Press Enter to continue...");
    displayimg("Files/riding.gif");
    attron(A_BOLD);
    grid1.print("Art and Animation by");
    attron(COLOR_PAIR(COLOR_GREEN));
    grid1.print("Abdul Ghani");
    attroff(COLOR_PAIR(COLOR_GREEN));
    attroff(A_BOLD);
    grid1.print("1823917");
    mvprintw(grid1.length, 0,"Press Enter to continue...");
    displayimg("Files/cluface.gif", false);
    attron(A_BOLD);
    grid1.print("Optimized by");
    attron(COLOR_PAIR(COLOR_RED));
    grid1.print("Ameera 'Eclipse'");
    attroff(COLOR_PAIR(COLOR_RED));
    attroff(A_BOLD);
    grid1.print("1823917");
    mvprintw(grid1.length, 0,"Press Enter to continue...");
    displayimg("Files/cluwalk.gif", false);

    grid1.print("Made for Dr. Rizal's EOP project.");
    displayimg("Files/riding.gif");
    attron(A_BOLD);
    attron(A_BLINK);
    init_pair(100,6,1);
    init_pair(101,6,2);
    attron(COLOR_PAIR(100));
    grid1.print("Enter the grid?",3);
    getchar();
    attroff(COLOR_PAIR(100));
    attron(COLOR_PAIR(101));
    grid1.print("Enter the grid?",3);
    attroff(COLOR_PAIR(101));
    attroff(A_BOLD);
    attroff(A_BLINK);
    displayimg("Files/riding.gif");
}

void chapter2(){
    clear();
    refresh();
    attron(COLOR_PAIR(COLOR_CYAN));
    grid1.print("'Dad?'",3);
    getchar();
    mvprintw(grid1.length/2+10, grid1.width-10, "'DAD?!'");
    refresh();
    getchar();
    attron(A_BOLD);
    mvprintw(grid1.length/2-1, grid1.width+10, "'DAD!!!'");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(COLOR_CYAN));
    refresh();
    getchar();
    grid1.print("You were certain that you heard your father's voice");
    grid1.print("But when you open your eyes, he is nowhere to be found");
    grid1.print("It must've been a dream.");
    grid1.print("An echo from the past of him telling you stories when you were a kid");
    grid1.print("Stories of him going to this amazing new world");
    grid1.print("A digital world; a new frontier.");
    grid1.print("He called it 'The grid'");
    grid1.print("God, the things he put in your head...");
}


int main() {
    nodelay(stdscr, TRUE);
    noecho();
    aiplay= true;
    initscr();
    InitCurses();
    init_pair(99,3,1);
    int l, w;
    getmaxyx(stdscr, l, w);
    grid1.grid_builder(l-1,w/3);
    //chapter1();
    //entertron();
    chapter2();
    //bikesgame();
    endwin();
    return 0;
}