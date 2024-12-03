
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
        sprites[i].offset = rand() % 4 + 18; //é pra aparecer só os carros
        sprites[i].data_register = i + 3;//verificar isso
        sprites[i].step_x = 5; //verificar esse valor
        sprites[i].step_y = 5; //verificar esse valor
        sprites[i].ativo = 1;
        sprites[i].collision = 0;
        sprites[i].last_update = clock();

        set_sprite(sprites[i].data_register, sprites[i].ativo, sprites[i].coord_x, sprites[i].coord_y, sprites[i].offset);
    }
}

void create_protectors(Sprite_Fixed *sprite2, unsigned int x2, unsigned int y2, unsigned int offset2){
    sprite2->coord_x = x2;             
    sprite2->coord_y = y2;               
    sprite2->offset = offset2;          
    sprite2->data_register = 2; 
    sprite2->ativo = 1;               

    set_sprite(sprite2->data_register, sprite2->ativo, sprite2->coord_x, sprite2->coord_y, sprite2->offset);
    
    /*int x1, unsigned int y1, unsigned int offset1,
    Sprite protector1;
    
    protector1.ativo = 1;
    protector1.collision = 0;
    protector1.coord_x = x1;
    protector1.coord_y = y1;
    protector1.data_register = 1;
    protector1.direction = ;
    protector1.last_update = clock();
    protector1.offset = offset1;
    protector1.step_x = ;
    protector1.step_y = ;*/

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

int collision(Sprite *sp1, Sprite_Fixed *sp2){//mudei o segundo para ser fixo temorareamnete dps deve voltar a ser sprite
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
        return 0; 
    } else {
        sp1->collision = 1;
        //Teria que colocar o do sp2 tbm quando ele voltar a ser movel
        return 1; 
    }
}
