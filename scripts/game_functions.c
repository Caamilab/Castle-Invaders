#include "game_functions.h"
#include <stdlib.h>
#include "gpu_lib.h"
#include <time.h>

void create_invaders(Sprite sprites[], int size) {
    int invaders_directions[] = {UP, UPPER_RIGHT, UPPER_LEFT};
    for (int i = 0; i < size; i++) {
        sprites[i].coord_x = rand() % (GAME_AREA_X_MAX - GAME_AREA_X_MIN) + GAME_AREA_X_MIN;
        sprites[i].coord_y = GAME_AREA_Y_MAX - 1;
        sprites[i].direction = invaders_directions[rand () % 3];
        sprites[i].offset = rand() % 2 + 21; //verificar 
        sprites[i].data_register = i + 6;
        sprites[i].step_x = rand() % 3 + 1; //mudei 
        sprites[i].step_y = rand() % 3 + 1; 
        sprites[i].ativo = 1;
        sprites[i].collision = 0;
        sprites[i].last_update = clock();

        set_sprite(sprites[i].data_register, sprites[i].ativo, sprites[i].coord_x, sprites[i].coord_y, sprites[i].offset);
    }
}

void create_protectors(Sprite *sprite1, unsigned int x1, unsigned int y1, unsigned int offset1, Sprite_Fixed *sprite2, unsigned int x2, unsigned int y2, unsigned int offset2){
    sprite2->coord_x = x2;             
    sprite2->coord_y = y2;               
    sprite2->offset = offset2;          
    sprite2->data_register = 2; 
    sprite2->ativo = 1;               

    set_sprite(sprite2->data_register, sprite2->ativo, sprite2->coord_x, sprite2->coord_y, sprite2->offset);
    
    sprite1->ativo = 1;
    sprite1->collision = 0;
    sprite1->coord_x = x1;
    sprite1->coord_y = y1;
    sprite1->data_register = 1;
    //protector1.direction = ;
    sprite1->last_update = clock();
    sprite1->offset = offset1;
   // protector1.step_x = ;
    //protector1.step_y = ;

    set_sprite(sprite1->data_register, sprite1->ativo, sprite1->coord_x, sprite1->coord_y, sprite1->offset);
    
}

void increase_coordinate(Sprite *sp){
    if (!sp->ativo) return;
    clock_t current_time = clock();
    double time_elapsed = ((double)(current_time - sp->last_update)) / CLOCKS_PER_SEC;

    if (time_elapsed >= 0.5) {
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

         if (sp->coord_x < GAME_AREA_X_MIN) {
            sp->coord_x = GAME_AREA_X_MIN;
        } else if (sp->coord_x >= GAME_AREA_X_MAX) {
            sp->coord_x = GAME_AREA_X_MAX - 1;
        }

        if (sp->coord_y < GAME_AREA_Y_MIN) {
            sp->coord_y = GAME_AREA_Y_MIN;
        } else if (sp->coord_y >= GAME_AREA_Y_MAX) {
            sp->coord_y = GAME_AREA_Y_MAX - 1;
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
         sp1->collision = 0;
         sp2->collision = 0;
        return 0; 
    } else {
        sp1->collision = 1;
        sp1->collision = 1;
        return 1; 
    }
}
