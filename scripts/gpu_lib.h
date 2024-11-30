#ifndef GPU_LIB_H
#define GPU_LIB_H

// Funções para mapear e desmapear memória
void video_open();
void video_close();

// Função para ler as chaves
void key_read();

// Funções para desenhar formas na tela
void draw_triangle(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void draw_square(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

// Funções para manipulação do background
void set_background_color(unsigned int, unsigned int, unsigned int);
void set_background_block(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

// Funções para manipulação de sprites
void set_sprite(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void add_sprite(unsigned int, unsigned int, unsigned int, unsigned int);

// Função para limpar a tela
void clear_screen();

#endif // GPU_LIB_H