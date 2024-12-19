 #include "gpu_lib.h"

extern void memory_map();
extern void memory_unmap();
extern int key();
extern void hexs(unsigned int digit0,unsigned int digit1,unsigned int digit2,unsigned int digit3,unsigned int digit4,unsigned int digit5);
extern void triangle(unsigned int color,unsigned int size,unsigned int x,unsigned int y,unsigned int address);
extern void square(unsigned int color,unsigned int size,unsigned int x,unsigned int y,unsigned int address);
extern void background_color(unsigned int red, unsigned int green, unsigned int blue);
extern void sprite(unsigned int reg, unsigned int activation_bit, unsigned int x, unsigned int y, unsigned int offset);
extern void background_block(unsigned int address, unsigned int red, unsigned int green, unsigned int blue);
extern void WSM(unsigned int address, unsigned int red, unsigned int green, unsigned int blue);

// funcao para mapear memoria
void video_open(){
    memory_map();
}

// funcao para desmapear memoria
void video_close(){
    memory_unmap();
}

//funcao para ler as chaves
int key_read(){
    return key();
}

// funcao para desenhar um triangulo na tela
void draw_triangle(unsigned int color,unsigned int size,unsigned int x,unsigned int y,unsigned int address){
  triangle(color, size, x, y, address);
}

// funcao para desenhar um quadrado na tela
void draw_square(unsigned int color,unsigned int size,unsigned int x,unsigned int y,unsigned int address){
  square(color, size, x, y, address);
}
// funcao para mudar a color do background
void set_background_color(unsigned int red, unsigned int green, unsigned int blue){
  background_color(red, green, blue);
}
// funcao para mostrar um sprite
void set_sprite(unsigned int reg, unsigned int activation_bit, unsigned int x, unsigned int y, unsigned int offset){
  sprite(reg, activation_bit, x, y, offset);
}
// funcao para mostrar bloco 8x8
void set_background_block(unsigned int lin, unsigned int col, unsigned int red, unsigned int green, unsigned int blue){
  unsigned int address = (lin * 80) + col;
  background_block(address, red, green, blue); //pensar no x e no y qual deve ser qual 
}
// funcao para adicionar sprite
void add_sprite(unsigned int base_address, unsigned int red, unsigned int green, unsigned int blue){
  for (unsigned int i = 0; i < 400; i++) {
    unsigned int address = base_address + i; 
    WSM(address, red, green, blue);           
  }
}
void clear_screen() {
  // Define a color de fundo para preto
  set_background_color(0, 0, 0);

  // Apaga todos os blocos de background
  for (int col = 0; col < 80; col++) {
    for (int lin = 0; lin < 60; lin++) { 
      set_background_block(lin, col, 110, 111, 111);  
    }
  }

  // Desabilita todos os sprites
  for (int i = 1; i < 32; i++) {
    set_sprite(i, 0, 0, 0, 0);  
  }

  // Desabilita todos os poligonos
  for (int i = 0; i< 16; i++){
    draw_triangle(0, 0 , 0, 0, i);
    draw_square(0, 0, 0, 0, i); 
  }
}//testar sem mudar e dps mudar para 15 e ver se resolve o problema 