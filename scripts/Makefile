# Nome do executável
TARGET = game

# Compilador
CC = gcc

# Flags de compilação
CFLAGS = -Wall -Wextra -std=c99 -g -pthread

# Arquivos fonte e headers
SRCS = gpu_lib.s gpu_lib.c game_functions.c game.c
HEADERS = gpu_lib.h game_functions.h

# Objetos gerados
OBJS = $(SRCS:.c=.o)

# Regra padrão
all: $(TARGET)

# Regra para gerar o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Regra para arquivos .c
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para arquivos .s
%.o: %.s
	as $< -o $@

# Limpeza dos arquivos gerados
clean:
	rm -f $(OBJS) $(TARGET)

# Executar o jogo
run: $(TARGET)
	./$(TARGET)
