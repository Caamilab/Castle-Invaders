#include "game_functions.h"
#include "gpu_lib.h"
#include "adxl345.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // Include for usleep

pthread_mutex_t accelMutex;

// Function prototype for monitor_accel
//void* monitor_accel(void* arg);

void* monitor_accel(void* arg) {
    int16_t XYZ[3];
    int16_t direction;
    uint8_t idAccel = ADXL345_ConfigureToGame();

    if (idAccel == 0xE5) { // correct address of adxl345
        ADXL345_Init();
        Sprite* protector1 = (Sprite*)arg;

        // Variáveis para armazenar a velocidade atual
        float current_step_x = 0.0;

        while (1) {
            pthread_mutex_lock(&accelMutex);

            if (ADXL345_WasActivityUpdated()) {
                // Atualiza os valores de aceleração
                ADXL345_XYZ_Read(XYZ);
                direction = movement((XYZ[0] * 4)); // 4 porque é o mg_per_lsb

                // Atualiza a direção e velocidade com base na aceleração detectada
                switch (direction) {
                    case 0: //centro
                        current_step_x = 0;
                        break;
                    case 1: // Direita
                        protector1->direction = RIGHT;
                        current_step_x = (XYZ[0] * 4) * 0.04; // Ajuste da velocidade
                        break;
                    case 2: // Esquerda
                        protector1->direction = LEFT;
                        current_step_x = - (XYZ[0] * 4) * 0.04; // Ajuste da velocidade
                        break;
                    default:
                        // Caso nenhuma direção seja detectada, mantém a velocidade atual
                        break;
                }

                // Atualiza o valor de step_x do sprite
                protector1->step_x = current_step_x;
            }

            pthread_mutex_unlock(&accelMutex);

            // Atualiza a posição do sprite continuamente com base na velocidade atual
            if (current_step_x != 0.0) {
                increase_coordinate(protector1);
            }

            usleep(10000); // Pequeno delay para evitar sobrecarga no processamento
        }
    }
    return NULL;
}

void create_wall(){
     
    unsigned int red = 128, green = 128, blue = 128;

    
    unsigned int y_top = 5; 

    
    unsigned int block_col_start = 0;   
    unsigned int block_col_end = 80;    

  
    for (unsigned int col = block_col_start; col <= block_col_end; col += 2) {
        set_background_block(y_top, col, red, green, blue);
    }

    for (unsigned int lin = y_top + 1; lin <= 15; lin++) { 
        for (unsigned int col = block_col_start; col <= block_col_end; col++) {
            set_background_block(lin, col, red, green, blue);
        }
    }
}

int main() {
    video_open();
    clear_screen();
    set_background_color(000, 100, 000);

    Sprite protector1;

    Sprite_Fixed protector2;
    create_protectors(&protector1, 310, 150, 0, &protector2, 310, 15, 1);
    
    pthread_t accelThread; 
    pthread_mutex_init(&accelMutex, NULL);

    // Create  thread 
    if (pthread_create(&accelThread, NULL, monitor_accel, (void*)&protector1) != 0) {
        perror("ERROR: could not create the button thread...");
        return 1;
    }

    srand(time(NULL));
    Sprite sprites[26];


    create_invaders(sprites, 26); // verificar quantos colocar na tela
    while (1) { 
        for (int i = 0; i < 26; i++) {
            increase_coordinate(&sprites[i]);
            if (collision(&sprites[i], &protector1)) {
                sprites[i].ativo = 0;
                set_sprite(sprites[i].data_register, 0, 0, 0, 0);
            }
        }
    }
    
    video_close(); 
    return 0;
}