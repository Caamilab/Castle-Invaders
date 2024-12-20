#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H

#include "gpu_lib.h"
#include <time.h>
#include <stdlib.h>

#define GAME_AREA_X_MIN 0
#define GAME_AREA_X_MAX 620
#define GAME_AREA_Y_MIN 100
#define GAME_AREA_Y_MAX 480

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480


#define LEFT 0
#define RIGHT 4
#define UP 2
#define DOWN 6
#define UPPER_RIGHT 1
#define UPPER_LEFT 3
#define BOTTOM_LEFT 5
#define BOTTOM_RIGHT 7

typedef struct {
    int coord_x, coord_y, offset;
    int data_register, ativo;
} Sprite_Fixed;

typedef struct {
    int coord_x, coord_y;
    int direction, offset, data_register;
    int step_x, step_y;
    int ativo, collision;
    clock_t last_update;
} Sprite;

void create_invaders(Sprite sprites[], int size);
void create_protectors(Sprite *sprite1, unsigned int x1, unsigned int y1, unsigned int offset1, Sprite_Fixed *sprite2, unsigned int x2, unsigned int y2, unsigned int offset2);
void increase_coordinate(Sprite *sp);
int collision(Sprite *sp1, Sprite *sp2);
void create_wall();

#endif // GAME_FUNCTIONS_H