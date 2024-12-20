#include "game_functions.h"
#include "gpu_lib.h"
#include "adxl345.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include <stdio.h>

//falta a lógica do END!!!!!!!!!!!!!!!!!!!!!!

pthread_mutex_t accelMutex;
pthread_mutex_t gameStateMutex;

 Sprite protector1;
 Sprite_Fixed protector2;
 Sprite sprites[26];

typedef enum {
    STOPPED,
    RUNNING,
    PAUSED,
    END
} GameState;

GameState currentGameState = STOPPED;
GameState previousState = END;

void* monitor_buttons() {
    int lastButtonValue = 0; // Armazena o último valor do botão

    while (1) {
        int buttonValue = key_read(); // Lê o valor atual do botão

        pthread_mutex_lock(&gameStateMutex);

        // Apenas processa mudanças no valor do botão
        if (buttonValue != lastButtonValue) {
            if (buttonValue == -1) { // Botão 1: Iniciar ou continuar o jogo
                if (currentGameState == STOPPED || currentGameState == PAUSED) {
                    currentGameState = RUNNING;
                }
            } else if (buttonValue == -2) { // Botão 2: Pausar o jogo
                if (currentGameState == RUNNING) {
                    currentGameState = PAUSED;
                }
            } else if (buttonValue == -4) { // Botão 4: Parar o jogo
                currentGameState = STOPPED;
            }

            else if (buttonValue == -8) { // Botão 4: Parar o jogo
                exit(0);
            }
        }

        lastButtonValue = buttonValue; // Atualiza o último valor do botão
        pthread_mutex_unlock(&gameStateMutex);

       // usleep(50000); // Pequeno delay para evitar polling excessivo
    }
    return NULL;
}

void* monitor_accel() {
    int16_t XYZ[3];
    int16_t direction;
    uint8_t idAccel = ADXL345_ConfigureToGame();

    if (idAccel == 0xE5) { // correct address of adxl345
        ADXL345_Init();
    
        // Variáveis para armazenar a velocidade atual
        float current_step_x = 0.0;

        while (1) {

            if (ADXL345_WasActivityUpdated()) {
                // Atualiza os valores de aceleração
                ADXL345_XYZ_Read(XYZ);
                direction = movement((XYZ[0] * 4)); // 4 porque é o mg_per_lsb

                pthread_mutex_lock(&accelMutex);
                // Atualiza a direção e velocidade com base na aceleração detectada
                switch (direction) {
                    case 0: //centro
                        current_step_x = 0;
                        break;
                    case 1: // Direita
                        protector1.direction = RIGHT;
                        current_step_x = (XYZ[0] * 4) * 0.04; // Ajuste da velocidade
                        break;
                    case 2: // Esquerda
                        protector1.direction = LEFT;
                        current_step_x = - (XYZ[0] * 4) * 0.04; // Ajuste da velocidade
                        break;
                    default:
                        current_step_x = 0;
                        // Caso nenhuma direção seja detectada, mantém a velocidade atual
                        break;
                } 
                // Atualiza o valor de step_x do sprite
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
            // Redefine o fundo e recria os invasores apenas na primeira vez ao entrar no estado RUNNING
            if (previousState != RUNNING) {
                pthread_mutex_lock(&accelMutex);
                //clear_screen(); // Limpa a tela
                //set_background_color(0, 100, 0); // Define a cor de fundo (verde)
                create_invaders(sprites, 26); // Recria os invasores
                pthread_mutex_unlock(&accelMutex);
            }

            // Renderiza o protetor
            pthread_mutex_lock(&accelMutex);
            set_sprite(protector1.data_register, protector1.ativo, protector1.coord_x, protector1.coord_y, protector1.offset);
            pthread_mutex_unlock(&accelMutex);
            //renderiza sprites
          for (int i = 0; i < 26; i++) {
                increase_coordinate(&sprites[i]);
                set_sprite(sprites[i].data_register, sprites[i].ativo, sprites[i].coord_x, sprites[i].coord_y, sprites[i].offset);
                if (collision(&sprites[i], &protector1)) {
                    sprites[i].ativo = 0;
                    set_sprite(sprites[i].data_register, 0, 0, 0, 0);
                }
            }
        } else if (state == STOPPED && previousState != STOPPED) {
            // Limpa a tela e reinicia elementos ao entrar no estado STOPPED
            pthread_mutex_lock(&accelMutex);
            clear_screen();
            create_protectors(&protector1, 310, 150, 0, &protector2, 310, 15, 1);
            set_background_color(0, 100, 0); // Define a cor de fundo (verde)
            create_wall(); // Cria as paredes
            pthread_mutex_unlock(&accelMutex);
        } else if (state == PAUSED) {
          //  usleep(10000); // Aguarda até que o estado mude para evitar loop excessivo
            continue;
        }

        previousState = state;
        //usleep(16000); // Aproximadamente ~60 FPS
    }
    return NULL;
}

int main() {
    video_open();
    
    pthread_t accelThread, buttonThread, renderThread;


    pthread_mutex_init(&accelMutex, NULL);
    pthread_mutex_init(&gameStateMutex, NULL);

    // Create  thread 
    if (pthread_create(&accelThread, NULL, monitor_accel, (void*)&protector1) != 0) {
        perror("ERROR: could not create the button thread...");
        return 1;
    }
    
    // Cria a thread para monitorar botões
    if (pthread_create(&buttonThread, NULL, monitor_buttons, NULL) != 0) {
        perror("ERROR: could not create the button thread...");
        return 1;
    }

     // Cria thread para renderizar a tela
    if (pthread_create(&renderThread, NULL, render_game, NULL) != 0) {
        perror("ERROR: could not create the render thread...");
        return 1;
    }

    pthread_join(accelThread, NULL);
    pthread_join(buttonThread, NULL);
    pthread_join(renderThread, NULL);

    video_close(); 
    return 0;
}


