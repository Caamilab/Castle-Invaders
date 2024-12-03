#include "game_functions.h"
#include "gpu_lib.h"
#include <stdlib.h>
#include <time.h>

int main() {
    Sprite_Fixed protector2;
    create_protectors(&protector2,50, 50, 0);
    srand(time(NULL));//pesquisar
    Sprite sprites[29];
    create_invaders(sprites, 29);
     while (1) {
        for (int i = 0; i < 29; i++) {
            increase_coordinate(&sprites[i]);
            if (collision(&sprites[i],&protector2)){
                sprites[i].ativo = 0;
                set_sprite(sprites[i].data_register, 0, 0, 0, 0);
            }
        }
     }
    return 0;
}