#include "game_functions.h"
#include "gpu_lib.h"
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));//pesquisar
    Sprite sprites[29];
    
    create_invaders(sprites, 29);
     while (1) {
        for (int i = 0; i < 29; i++) {
            increase_coordinate(&sprites[i]);
        }
     }
    return 0;
}