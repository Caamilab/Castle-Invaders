#include <stdio.h>
#include <unistd.h>
#include "game_functions.h"
#include "gpu_lib.h"
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>  
#include <pthread.h>

/* screen limits */
#define XMIN 0
#define XMAX 640
#define NYMAX 112
#define YMIN 0 
#define YMAX 480 

int fd, x, y, deltaX, deltaY;
int a, i, fd, bytes,left, middle, right;
unsigned char data[3];
const char *pDevice = "/dev/input/mice";
pthread_mutex_t mouse_mutex;

void *read_mouse(void *arg) {
    while (1) {
        pthread_mutex_lock(&mouse_mutex);
        int bytes = read(fd, data, sizeof(data)); 
        if (bytes > 0) {
            left = data[0] & 0x1;
            right = data[0] & 0x2;
            middle = data[0] & 0x4;

            x = (signed char)data[1];
            y = (signed char)data[2];

            deltaX += x;
            deltaY -= y;

            if (deltaX > XMAX) deltaX = XMAX;
            if (deltaX < XMIN) deltaX = XMIN;
            if (deltaY > NYMAX) deltaY = NYMAX;
            if (deltaY < YMIN) deltaY = YMIN;
        }
        pthread_mutex_unlock(&mouse_mutex);
        usleep(5000); 
    }
    return NULL;
}

void * shot(void* arg){
    int a, deltaXAux, deltaYAux;
    while(1){
        if (left == 1){
            deltaXAux = deltaX; 
            deltaYAux = deltaY+20;
            printf("Delta x - %d\n",deltaX);
            printf("AuxDelta x - %d\n",deltaX);
            for (a = deltaYAux; a <= YMAX; a++ ){
                set_sprite(6, 1, deltaXAux, a, 11);
                usleep(5000);
            } 
        } 
    }
    return NULL; 
    
}


int main() { 
    pthread_t threadShot, threadMouse; 

    deltaX = 320; //320 meio 
    deltaY = YMIN; //240 meio
    
    memory_map(); 

    Sprite protector1; // cara que recebe colisao e joga com acelerometro  
    Sprite_Fixed protector2; // cara que nao recebe colisão e joga com mouse

    Sprite invaders[26]; // invasores  

    pthread_mutex_init(&mouse_mutex, NULL);

    
    create_protectors(&protector1, deltaX, deltaY, 1, &protector2, deltaX, 15, 0);

    fd = open(pDevice, O_RDWR);
    if (fd == -1){
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }

    clear_screen();
    if(pthread_create(&threadShot, NULL, shot, NULL) != 0){
        perror("Erro ao criar a thread do tiro");
        exit(EXIT_FAILURE);
    } 

    if (pthread_create(&threadMouse, NULL, read_mouse, NULL) != 0) {
        perror("Erro ao criar a thread do mouse");
        exit(EXIT_FAILURE);
    }
    
    set_background_color(000, 100, 000);
    //set_sprite(3, 1, deltaX, deltaY, 7); 
    //set_sprite(3, 1, deltaX, deltaY+20, 1); 
    //set_sprite(6, 1, deltaX+10, deltaY+20, 11);
    set_background_block(14, 0, 000, 000, 000); //linha, coluna, r, g, b

    while(1){
        pthread_mutex_lock(&mouse_mutex);
        set_sprite(2, 1, deltaX, deltaY, 1);
        pthread_mutex_unlock(&mouse_mutex);
        usleep(16000); // 60 fps
        create_protectors(&protector1, 310, 150, 0, &protector2, 310, 15, 1);
        //create_invaders(sprites, 26);

        //printf("x - %d\n",deltaX);
        //printf("y - %d\n",deltaY);
        //printf("left - %d\n",left);

        //printf("x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);
        }   

    close(fd);
    pthread_mutex_destroy(&mouse_mutex);
    return 0;
} 
    /*Sprite_Fixed protector2;
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
     }*/