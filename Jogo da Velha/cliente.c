 #include <stdio.h>	
 #include <unistd.h>
 #include <stdlib.h>
 #include <errno.h>	
 #include <string.h>	
 #include <sys/types.h>	
 #include <sys/socket.h>	
 #include <netinet/in.h>	
 #include <arpa/inet.h>	
 #include <netdb.h>	
 #include <time.h>


struct mensagem
{
	char caractere;
	int situacaoJogo;
	char nome_adversario[20];
	char nome[20];
	char tabuleiro[9];
	char msg[40];
};

 /*	
 * Esta função reporta erro e finaliza do programa
 */	
 static void	
	bail(const char *on_what) {	
	if ( errno != 0 ) {	
		fputs(strerror(errno),stderr);	
		fputs("	 ",stderr);
	}	
	fputs(on_what,stderr);	
	fputc('\n',stderr);	
	exit(1);	
 }
	
void print_matriz(char tabuleiro[9]) {
	int linha = 0;
	for (int i = 0; i < 9; i++){
		if (linha == 3){
			printf("\n");
			linha = 0;
		}
		printf("%c   ", tabuleiro[i]);
		linha = linha + 1;
	}
	printf("\n");
}
	
 int main(int argc,char *argv[]) {	
	int z;
	char *srvr_addr = NULL;
	srvr_addr = "127.0.0.1";
	char *srvr_port = "9999";
	struct sockaddr_in adr_srvr;/* AF_INET */
	int len_inet; /* comprimento */
	int s; /* Socket */
	//int n;
	int x;
	struct mensagem jogo;
	jogo.situacaoJogo = 0;
	int inseriu = 0;

	/*		
	* Cria um socket do tipo TCP		
	*/		
	s = socket(PF_INET,SOCK_STREAM,0);		
	if ( s == -1 )
		bail("socket()");

	/*
	* Preenche a estrutura do socket com a porta e endereÃ§o do servidor
	*/
	memset(&adr_srvr,0,sizeof adr_srvr);
	adr_srvr.sin_family = AF_INET;
	adr_srvr.sin_port = htons(atoi(srvr_port));
	adr_srvr.sin_addr.s_addr =
	inet_addr(srvr_addr);
	if ( adr_srvr.sin_addr.s_addr == INADDR_NONE )
		bail("bad address.");

	printf("Digite o seu nome: ");
	scanf("%s", jogo.nome);
	printf("Aguardando adversário conectar!\n");
	
		
	 /*
	 * Conecta com o servidor
	 */
	 len_inet = sizeof adr_srvr;

	 z = connect(s,(struct sockaddr *) &adr_srvr,len_inet);
	 if ( z == -1 )
	 bail("connect(2)");
 	 //conecta servidor


	 //Escreve no Socket do Servidor
	 z = write(s,(const void *) &jogo,sizeof(struct mensagem));
	 if ( z == -1 )
		bail("write(2)");
	 //termina de escrever no Socket do Servidor
	

	 z = -1;
	while(z == -1)
	{
	 	//lê mensagem do servidor
		z = read(s,&jogo,sizeof(struct mensagem));
		
		if ( z == -1 )
			 bail("read(2)");
		//termina de lê a mensagem do servidor
		
	}
	 
	 printf("%s\n", jogo.msg);
	 	
	while(1) {

		system("clear");
		printf("Você: %s\nAdversário: %s\n", jogo.nome, jogo.nome_adversario);
		print_matriz(jogo.tabuleiro);
		printf("Aguarde o oponente jogar!\n");
		z = read(s,&jogo,sizeof(struct mensagem));
		if (jogo.situacaoJogo != 0){
			system("clear");
			break;
		}
		system("clear");
		printf("Você: %s\nAdversário: %s\n", jogo.nome, jogo.nome_adversario);
		print_matriz(jogo.tabuleiro);

		while (inseriu == 0){
			printf("Escreva sua jogada: ");
			scanf("%d", &x);
			if (jogo.tabuleiro[x - 1] != 'X' && jogo.tabuleiro[x - 1] != 'O'){
				jogo.tabuleiro[x - 1] = jogo.caractere;		
				inseriu = 1;
			}	
			else{
					system("clear");
					printf("Posição %d ocupada!!\n", x);	
				    print_matriz(jogo.tabuleiro);
			}
		}
			inseriu = 0;		

		

		z = write(s,(void *) &jogo, sizeof(jogo));

	}
		

	if (jogo.situacaoJogo == 1){
		printf("PARABÉNS %s VOCÊ GANHOU!!!\n", jogo.nome);
		printf("%s DERROTADO!\n", jogo.nome_adversario);
		print_matriz(jogo.tabuleiro);
	}
	else if (jogo.situacaoJogo == 2){
		printf("O JOGO EMPATOU!!\n");
		print_matriz(jogo.tabuleiro);
	}
	else{
		printf("VOCÊ PERDEU!\n");
		printf("%s VENCEU!\n", jogo.nome_adversario);
		print_matriz(jogo.tabuleiro);
	}
			
	
	//sleep(30);
	close(s);	
	putchar('\n');
		
	return 0;	
}
