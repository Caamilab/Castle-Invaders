
#include "game_functions.h"
#include <stdlib.h>
#include "gpu_lib.h"
#include <time.h>

void create_invaders(Sprite sprites[], int size) {
    int invaders_directions[] = {UP, UPPER_RIGHT, UPPER_LEFT};
    for (int i = 0; i < size; i++) {
        sprites[i].coord_x = rand() % 640;
        sprites[i].coord_y = 479;
        sprites[i].direction = invaders_directions[rand () % 3];
        sprites[i].offset = rand() % 24; //mudar esse valor dps
        sprites[i].data_register = i + 2;
        sprites[i].step_x = 5; //verificar esse valor
        sprites[i].step_y = 5; //verificar esse valor
        sprites[i].ativo = 1;
        sprites[i].collision = 0;
        sprites[i].last_update = clock();

        set_sprite(sprites[i].data_register, sprites[i].ativo, sprites[i].coord_x, sprites[i].coord_y, sprites[i].offset);
    }
}

void increase_coordinate(Sprite *sp){
    if (!sp->ativo) return;
    clock_t current_time = clock();
    double time_elapsed = ((double)(current_time - sp->last_update)) / CLOCKS_PER_SEC;
    if (time_elapsed >= 1.0) {
        switch (sp->direction) {
           case UP:
                sp->coord_y -= sp->step_y;
                break;
            case UPPER_RIGHT:
                sp->coord_x += sp->step_x;
                sp->coord_y -= sp->step_y;
                break;
            case UPPER_LEFT:
                sp->coord_x -= sp->step_x;
                sp->coord_y -= sp->step_y;
                break;
            case DOWN:
                sp->coord_y += sp->step_y;
                break;
            case BOTTOM_RIGHT:
                sp->coord_x += sp->step_x;
                sp->coord_y += sp->step_y;
                break;
            case BOTTOM_LEFT:
                sp->coord_x -= sp->step_x;
                sp->coord_y += sp->step_y;
                break;
            case LEFT:
                sp->coord_x -= sp->step_x;
                break;
            case RIGHT:
                sp->coord_x += sp->step_x;
                break;
            default:
                break;
        }

         if (sp->coord_x < 0) {
            sp->coord_x = 0;
        } else if (sp->coord_x >= SCREEN_WIDTH) {
            sp->coord_x = SCREEN_WIDTH - 1;
        }

        if (sp->coord_y < 0) {
            sp->coord_y = 0;
        } else if (sp->coord_y >= SCREEN_HEIGHT) {
            sp->coord_y = SCREEN_HEIGHT - 1;
        }

        sp->last_update = current_time;
        set_sprite(sp->data_register, sp->ativo, sp->coord_x, sp->coord_y, sp->offset);
    }

}

int collision(Sprite *sp1, Sprite *sp2){
    const int sprite_width = 20;
    const int sprite_height = 20;

    int left1 = sp1->coord_x;
    int right1 = sp1->coord_x + sprite_width;
    int top1 = sp1->coord_y;
    int bottom1 = sp1->coord_y + sprite_height;

    int left2 = sp2->coord_x;
    int right2 = sp2->coord_x + sprite_width;
    int top2 = sp2->coord_y;
    int bottom2 = sp2->coord_y + sprite_height;

     if (right1 < left2 || left1 > right2 || bottom1 < top2 || top1 > bottom2) {
        return 0; 
    } else {
        return 1; 
    }
}
