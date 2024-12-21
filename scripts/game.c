#include <stdio.h>
#include <unistd.h>
#include "game_functions.h"
#include "gpu_lib.h"
#include "adxl345.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include <stdio.h>
#include <fcntl.h> 


/* screen limits */
#define XMIN 0
#define XMAX 640
#define NYMAX 36
#define YMIN 0 
#define YMAX 480 

int fd, x, y, deltaX, deltaY;
int a, i, bytes,left, middle, right;
unsigned char data[3];
const char *pDevice = "/dev/input/mice";

pthread_mutex_t accelMutex;
pthread_mutex_t gameStateMutex;
pthread_mutex_t mouse_mutex; 

 Sprite protector1;
 Sprite protector2;  
 Sprite shotSprite; 
 Sprite sprites[20];

typedef enum {
    STOPPED,
    RUNNING,
    PAUSED,
    END
} GameState;

GameState currentGameState = STOPPED;
GameState previousState = END;

void *read_mouse(void *arg) {

    fd = open(pDevice, O_RDWR);
    if (fd == -1){
        printf("ERROR Opening %s\n", pDevice);
        return -1;
    }
    while (1) {
        pthread_mutex_lock(&mouse_mutex);
        int bytes = read(fd, data, sizeof(data)); 
        if (bytes > 0) {
            left = data[0] & 0x1;
            right = data[0] & 0x2;
            middle = data[0] & 0x4;

            x = (signed char)data[1];
            y = (signed char)data[2];

            printf("x - %d\n",x);
            printf("y - %d\n",y);

            deltaX += x;
            deltaY -= y;

            if (deltaX > XMAX) deltaX = XMAX;
            if (deltaX < XMIN) deltaX = XMIN;
            if (deltaY > NYMAX) deltaY = NYMAX;
            if (deltaY < YMIN) deltaY = YMIN;
        }
        protector2.coord_x = deltaX;
        protector2.coord_y = deltaY;
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
                shotSprite.data_register = 6;
                shotSprite.coord_x = deltaXAux;
                shotSprite.coord_y = a;
                shotSprite.ativo = 1;
                shotSprite.offset = 11; 
                set_sprite(shotSprite.data_register, shotSprite.ativo, shotSprite.coord_x, shotSprite.coord_y, shotSprite.offset);
                usleep(3000);
            } 
        } 
    }
    return NULL; 
    
}

void* monitor_buttons() {
    int lastButtonValue = 0; 

    while (1) {
        int buttonValue = key_read(); 

        pthread_mutex_lock(&gameStateMutex);

        if (buttonValue != lastButtonValue) {
            if (buttonValue == -1) {
                if (currentGameState == STOPPED || currentGameState == PAUSED) {
                    currentGameState = RUNNING;
                }
            } else if (buttonValue == -2) { 
                if (currentGameState == RUNNING) {
                    currentGameState = PAUSED;
                }
            } else if (buttonValue == -4) { 
                currentGameState = STOPPED;
            }

            else if (buttonValue == -8) { 
                clear_screen();
                exit(0);
            }
        }

        lastButtonValue = buttonValue; 
        pthread_mutex_unlock(&gameStateMutex);
    }
    return NULL;
}

void* monitor_accel() {
    int16_t XYZ[3];
    int16_t direction;
    uint8_t idAccel = ADXL345_ConfigureToGame();

    if (idAccel == 0xE5) { 
        ADXL345_Init();
    
        float current_step_x = 0.0;

        while (1) {

            if (ADXL345_WasActivityUpdated()) {
                
                ADXL345_XYZ_Read(XYZ);
                direction = movement((XYZ[0] * 4)); 

                pthread_mutex_lock(&accelMutex);
                
                switch (direction) {
                    case 0: 
                        current_step_x = 0;
                        break;
                    case 1: 
                        protector1.direction = RIGHT;
                        current_step_x = (XYZ[0] * 4) * 0.04; 
                        break;
                    case 2: 
                        protector1.direction = LEFT;
                        current_step_x = - (XYZ[0] * 4) * 0.04; 
                        break;
                    default:
                        current_step_x = 0;
                        break;
                } 
                
                protector1.step_x = current_step_x;
                increase_coordinate(&protector1);
                pthread_mutex_unlock(&accelMutex);   
            }

        }
    }
    return NULL;
}

void* render_game() {
    srand(time(NULL));

    while (1) {
        pthread_mutex_lock(&gameStateMutex);
        GameState state = currentGameState;
        pthread_mutex_unlock(&gameStateMutex);

        if (state == RUNNING) {
        
            if (previousState != RUNNING) {
                pthread_mutex_lock(&accelMutex);
                create_invaders(sprites, 20); 
                pthread_mutex_unlock(&accelMutex);
            }

            pthread_mutex_lock(&accelMutex);
            set_sprite(protector1.data_register, protector1.ativo, protector1.coord_x, protector1.coord_y, protector1.offset);
            pthread_mutex_unlock(&accelMutex);

            pthread_mutex_lock(&mouse_mutex);
            printf("cordenada - %d\n",protector2.coord_y);
            set_sprite(protector2.data_register, protector2.ativo, protector2.coord_x, protector2.coord_y, protector2.offset);
            pthread_mutex_unlock(&mouse_mutex);

          for (int i = 0; i < 20; i++) {
                increase_coordinate(&sprites[i]);
                set_sprite(sprites[i].data_register, sprites[i].ativo, sprites[i].coord_x, sprites[i].coord_y, sprites[i].offset);
                if (collision(&sprites[i], &protector1)) {
                    sprites[i].ativo = 0;
                    set_sprite(sprites[i].data_register, 0, 0, 0, 0);
                }
                if (collision(&sprites[i], &shotSprite)){
                    sprites[i].ativo = 0;
                    set_sprite(sprites[i].data_register, 0, 0, 0, 0);
                    set_sprite(shotSprite.data_register, 0, 0, 0, 0); 
                }

            }
        } else if (state == STOPPED && previousState != STOPPED) {

            pthread_mutex_lock(&accelMutex);
            clear_screen();
            create_protectors(&protector1, 310, 150, 0, &protector2, 310, 15, 1);
            set_background_color(0, 100, 0); 
            create_wall(); 
            pthread_mutex_unlock(&accelMutex);
        } else if (state == PAUSED) {
            continue;
        }

        previousState = state;
    }
    return NULL;
}

int main() {
    video_open();
    
    pthread_t accelThread, buttonThread, renderThread, threadShot, threadMouse;
    deltaX = 320; 
    deltaY = YMIN; 

    pthread_mutex_init(&accelMutex, NULL);
    pthread_mutex_init(&gameStateMutex, NULL);
    pthread_mutex_init(&mouse_mutex, NULL);

    if (pthread_create(&accelThread, NULL, monitor_accel, (void*)&protector1) != 0) {
        perror("ERROR: could not create the button thread...");
        return 1;
    }

    if (pthread_create(&buttonThread, NULL, monitor_buttons, NULL) != 0) {
        perror("ERROR: could not create the button thread...");
        return 1;
    }

    if (pthread_create(&renderThread, NULL, render_game, NULL) != 0) {
        perror("ERROR: could not create the render thread...");
        return 1;
    }

    if(pthread_create(&threadShot, NULL, shot, NULL) != 0){
        perror("Erro ao criar a thread do tiro");
        exit(EXIT_FAILURE);
    } 

    if (pthread_create(&threadMouse, NULL, read_mouse, NULL) != 0) {
        perror("Erro ao criar a thread do mouse");
        exit(EXIT_FAILURE);
    }

    video_close(); 
    return 0;
}


