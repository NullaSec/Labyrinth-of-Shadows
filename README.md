# Labyrinth of Shadows
Projeto final de Laboratórios de Algoritmia I feito em C e baseado no estilo "Rouguelite"
### Info:
O `Makefile` é o ficheiro que tem os comando para correr o jogo, não precisam de editar que já está feito.

A `src` é a pasta que tem o código do jogo e os headers (pastas que acabam em .h usadas para partilhar funções entre os ficheiros).

O `README.md` é isto que estão a ler agora.


### Como atualizar o código no repositório
Caso a opção Add File > Upload Files não vos apareça façam o que diz no vídeo:

https://www.youtube.com/watch?v=LxkXT9WD7yk

### Como correr o jogo (SÓ LINUX):
- 1º: Transferir os ficheiros
- 2º: Pôr a pasta onde quiserem
- 3º: No terminal usar o comando `cd` (change directory) para chegar ao local onde puseram a pasta

	Ex.: O meu jogo está na pasta LabyrinthOfShadows que está no ambiente de trabalho, por isso tenho de fazer:
  
  `cd Desktop/LabyrinthOfShadows` para chegar até à pasta.
  
	Também podem fazer por passos:
  
	`cd Desktop`
  
	`cd LabyrinthOfShadows`
  
	Para terem a certeza que estão no local correto usem o comando `ls` para mostrar os conteúdos da pasta onde estão.
  
	`ls` na pasta correta deve mostrar ` Makefile    src` ou algo do género.
	
- 4º: Basta escrever `make` quando estiverem dentro da pasta e o jogo deve começar.

### NOTA: USEM SEMPRE "Q" PARA SAIR DO JOGO SENÃO NÃO APAGA OS FICHEIROS QUE CRIA AO CORRER O JOGO
