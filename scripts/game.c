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

//falta a lógica do END!!!!!!!!!!!!!!!!!!!!!!

/* screen limits */
#define XMIN 0
#define XMAX 640
//#define NYMAX 112
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
 Sprite protector2;  //2 era o fixo
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
                clear_screen();
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
                create_invaders(sprites, 20); // Recria os invasores
                pthread_mutex_unlock(&accelMutex);
            }

            // Renderiza o protetor
            pthread_mutex_lock(&accelMutex);
            set_sprite(protector1.data_register, protector1.ativo, protector1.coord_x, protector1.coord_y, protector1.offset);
            pthread_mutex_unlock(&accelMutex);

            pthread_mutex_lock(&mouse_mutex);
            printf("cordenada - %d\n",protector2.coord_y);
            set_sprite(protector2.data_register, protector2.ativo, protector2.coord_x, protector2.coord_y, protector2.offset);
            pthread_mutex_unlock(&mouse_mutex);

            //renderiza sprites
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
    
    pthread_t accelThread, buttonThread, renderThread, threadShot, threadMouse;
    deltaX = 320; //320 meio 
    deltaY = YMIN; //240 meio

    pthread_mutex_init(&accelMutex, NULL);
    pthread_mutex_init(&gameStateMutex, NULL);
    pthread_mutex_init(&mouse_mutex, NULL);

 

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

    if(pthread_create(&threadShot, NULL, shot, NULL) != 0){
        perror("Erro ao criar a thread do tiro");
        exit(EXIT_FAILURE);
    } 

    if (pthread_create(&threadMouse, NULL, read_mouse, NULL) != 0) {
        perror("Erro ao criar a thread do mouse");
        exit(EXIT_FAILURE);
    }

    
    pthread_join(accelThread, NULL);
    pthread_join(buttonThread, NULL);
    pthread_join(renderThread, NULL);

    video_close(); 
    return 0;
}


