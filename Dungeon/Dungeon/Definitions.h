#ifndef __DEFINITION_H
#define __DEFINITION_H




const int W = 600; // window width
const int H = 600; // window height
const int NUM_ROOMS = 10;
const int MAX_RAND = 2;

const int SPACE = 1;
const int WALL = 2;
const int VISITED_W1 = 3;
const int VISITED_W2 = 4;
const int GRAY = 5;
const int MEDICINE = 6;
const int AMMO = 7;
const int WARRIOR_1 = 8;
const int WARRIOR_2 = 9;
const int WARRIOR1_PATH = 10;
const int WARRIOR2_PATH = 11;

//const enum WARRIOR1_COLORS {WARRIOR = 8, PATH = 10, VISITED = 3};
//const enum WARRIOR2_COLORS {WARRIOR = 9, PATH = 11, VISITED = 4};


const int MSIZE = 100;
const double SQSIZE = 2.0 / MSIZE;

const int UP = 1;
const int DOWN = 2;
const int LEFT = 3;
const int RIGHT = 4;

const int MAX_NUM_OF_MEDICINE = 7;
const int MAX_NUM_OF_AMMO = 4;

const int MIN_ROOM_WIDTH_AND_HEIGHT = 5;
const int MAX_ROOM_WIDTH = 15;
const int MAX_ROOM_HEIGHT = 25;

const int MAX_WARRIOR_HP = 50;
const int MAX_WARRIOR_BULLETS = 30;

const int MAX_HIT_DAMAGE = 25;
const int MIN_HIT_DAMAGE = 5;

const int PIXEL_RADIUS = 3;

#endif // !__DEFINITION_H