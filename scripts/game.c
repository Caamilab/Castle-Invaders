#include "game_functions.h"
#include "gpu_lib.h"
#include "adxl345.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>


pthread_mutex_t accelMutex;
//Deixar so o prototipo aqui 
void* monitor_accel(void* arg){
    int16_t XYZ[3];
    int16_t direction;
    uint8_t idAccel = ADXL345_ConfigureToGame();

    if (idAccel == 0xE5){ // correct address of adxl345
        ADXL345_Init();
        while (1)
        {
            pthread_mutex_lock(&accelMutex);

            if (ADXL345_WasActivityUpdated()){
                ADXL345_XYZ_Read(XYZ);
                direction = movement((XYZ[0]*4)); //4 because of the mg_per_lsb
                Sprite* protector1 = (Sprite*)arg; 
                
                switch (direction) { 
                    case 1: 
                        protector1->direction = RIGHT;
                        break;
                    case 2: 
                        protector1->direction = LEFT;
                        break;
                }
                
                protector1->step_x = (XYZ[0]*4) * 0.01;
                increase_coordinate(&protector1);
            }  
            pthread_mutex_unlock(&accelMutex);
            usleep(10000);      
        }                   
    }
    return NULL; 
}

int main() {
    video_open();
    
    set_background_color(255, 255, 255);

    Sprite protector1;
    Sprite_Fixed protector2;
    create_protectors(&protector1, 220, 120, 0, &protector2, 220, 0, 1);
    
    srand(time(NULL));
    Sprite sprites[26];


    pthread_t accelThread; 
    pthread_mutex_init(&accelMutex, NULL);

    // create the button thread 
    if (pthread_create(&accelThread, NULL, monitor_accel, (void*)&protector1) != 0) {
        perror("ERROR: could not create the button thread...");
        return 1;
    }

    create_invaders(sprites, 26);//mudar para que caiba o sprite do mouse
     while (1) { 
        for (int i = 0; i < 26; i++) {
            increase_coordinate(&sprites[i]);
            if (collision(&sprites[i],&protector1)){
                sprites[i].ativo = 0;
                set_sprite(sprites[i].data_register, 0, 0, 0, 0);
            }
        }
     }
    video_close(); 
    return 0;
}
//Consertar o clear_screen
// Codigo castelos 
/*#include <stdio.h>

// Suponha que esta função está definida em outro lugar
void set_background_block(unsigned int x, unsigned int y, unsigned int red, unsigned int green, unsigned int blue);

int main() {
    // Definindo a cor cinza para as torres
    unsigned int red = 128, green = 128, blue = 128;

    // Altura do topo das torres
    unsigned int y_top = 30;

    // Coordenadas e larguras das torres
    unsigned int tower1_x_start = 30;
    unsigned int tower1_x_end = 180;

    unsigned int tower2_x_start = 200;
    unsigned int tower2_x_end = 350;

    unsigned int tower3_x_start = 370;
    unsigned int tower3_x_end = 520;

    // Desenhar blocos alternados no topo da primeira torre
    for (unsigned int x = tower1_x_start; x <= tower1_x_end; x += 2) {
        set_background_block(x, y_top, red, green, blue);
    }

    // Desenhar blocos alternados no topo da segunda torre
    for (unsigned int x = tower2_x_start; x <= tower2_x_end; x += 2) {
        set_background_block(x, y_top, red, green, blue);
    }

    // Desenhar blocos alternados no topo da terceira torre
    for (unsigned int x = tower3_x_start; x <= tower3_x_end; x += 2) {
        set_background_block(x, y_top, red, green, blue);
    }

    // Desenhar o corpo das torres
    for (unsigned int y = y_top + 1; y <= 80; y++) {
        for (unsigned int x = tower1_x_start; x <= tower1_x_end; x++) {
            set_background_block(x, y, red, green, blue);
        }
        for (unsigned int x = tower2_x_start; x <= tower2_x_end; x++) {
            set_background_block(x, y, red, green, blue);
        }
        for (unsigned int x = tower3_x_start; x <= tower3_x_end; x++) {
            set_background_block(x, y, red, green, blue);
        }
    }

    return 0;
}
*/