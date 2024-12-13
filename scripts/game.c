#include <stdio.h>
#include <unistd.h>
#include "game_functions.h"
#include "gpu_lib.h"
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>  

/* screen limits */
#define XMIN 0
#define XMAX 640
#define NYMAX 112
#define YMIN 0 
#define YMAX 480 


int main() {
    int i, fd, bytes,left, middle, right;
    unsigned char data[3];
    int x, y, deltaX, deltaY;
    const char *pDevice = "/dev/input/mice";

    deltaX = 320; //320 meio 
    deltaY = YMIN; //240 meio
    
    memory_map(); 

    fd = open(pDevice, O_RDWR);
    if (fd == -1){
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }
    clear_screen(); 
    set_background_color(000, 111, 000);
    set_sprite(3, 1, deltaX, deltaY, 7); 
    set_background_block(14, 0, 000, 000, 000); //linha, coluna, r, g, b

    while(1){
        bytes = read(fd, data, sizeof(data)); 
        if(bytes > 0){
            left = data[0] & 0x1;
            right = data[0] & 0x2;
            middle = data[0] & 0x4;

            x = (signed char)data[1];
            y = (signed char)data[2];
            
            /*  esqueda = -x 
                direita = +x
                cima = +y
                baixo = -y
            */
            deltaX = deltaX + x;
            printf("x - %d\n",deltaX);
            deltaY = deltaY - y; 
            printf("y - %d\n",deltaY);
            printf("left - %d\n",left);
            if (deltaX > XMAX) deltaX = XMAX;
            if (deltaX < XMIN) deltaX = XMIN;
            if (deltaY > NYMAX) deltaY = NYMAX;
            if (deltaY < YMIN) deltaY = YMIN;

            set_sprite(3, 1, deltaX, deltaY, 7);  

            /*if (y >= YMIN && y <= YMAX){
                x = deltaX; 
                y = deltaY + y; 
                deltaY = y;
                draw_triangle(0b000000111, 0b0001, deltaY, deltaX, 0b0001);
                printf("Y limits\n");
            }*/

            
            //printf("x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);
        }   
    }
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