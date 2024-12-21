# :space_invader: :european_castle: :video_game: CASTLE INVADERS :video_game: :european_castle: :space_invader:

## Equipe:
- [Camila de Araújo Bastos](https://github.com/Caamilab)
- [Fernanda Marinho Silva](https://github.com/Fernanda-Marinho/)

## Executando o jogo 
### Clone do repositório
``` bash
git clone https://github.com/Caamilab Castle-Invaders 
``` 

### Execução do jogo
``` bash
cd Castle-Invaders/scripts/
make 
make run   
```

## Como jogar 
Para jogar CASTLE INVADERS, é necessário ter acesso à plataforma de desenvolvimento DE1-SoC disponível no Laboratório de Eletrônica Digital e Sistemas (LEDS) da Universidade Estadual de Feira de Santana (UEFS) e um mouse. 
<p align="center">
<img src="media/de1soc.jpg" alt="DE1-SoC" width="300"/> 
</p>
O jogo é composto por dois jogadores: o BLUESPRITE e o REDSPRITE. 

### BLUESPRITE

O jogador fica na parte de baixo do castelo.\
Para movimentação será necessário utilizar o acelerômetro presente na DE1-SoC.\
O objetivo é mover o sprite (direita - esquerda) até que ele atinja um invasor.  
<p align="center">
<img src="media/bluesprit.png" alt="Bluesprit" width="300"/> 
</p>

### REDSPRITE

O jogador fica na parte de cima do castelo.\
Para movimentação será necessário utilizar o mouse.\
O objetivo é mover o sprite (direita - esquerda) e clicar com o botão esquerdo para soltar um tiro e atingir o invasor. \
Não é permitido mais de um tiro por vez.  
<p align="center">
<img src="media/redsprit.png" alt="Redsprite" width="300"/> 
</p>

### INVADERS

Nenhum jogador pode ser invasor. \
Tem como objetivo atacar o castelo. \
Os jogadores precisam destruí-los.  
<p align="center">  
<img src="media/invaders.png" alt="Invaders" width="300"/> 
</p>


### INÍCIO, PAUSA E FIM

Para iniciar, pausar, retomar e encerrar o jogo, é necessário usar os botões dispostos na placa. \
Cada botão tem uma funcionalidade diferente.
<p align="center">  
<img src="media/buttons.png" alt="Buttons" width="300"/> 
</p>

| Botão  |   Ação            |                           Descrição                                    |
| -----  | --------          |--------------------------------------------------------------------  |
| B0     | Iniciar           | Inicia o jogo. É obrigatório o uso deste botão para iniciar o jogo.    |
| B1     | Pausar/Retomar| Pausa o jogo em determinado ponto e, se clicado novamente, retoma.     |
| B2     | Reiniciar| Reinicia o jogo do independente do momento que for acionado.           |
| B3     | Encerrar| Mata o processo e fecha o jogo.                                        |