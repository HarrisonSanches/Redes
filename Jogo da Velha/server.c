#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


struct mensagem
{
	char caractere;
	int situacaoJogo;
	char nome_adversario[20];
	char nome[20];
	char tabuleiro[9];
	char msg[40];
};

int verificaDiagonais(char array[9], char a){
	int verificador = 0;
	//printf("Diagonal1: %c  %c  %c\n", array[0], array[4], array[8]);
	//printf("Diagonal2: %c  %c  %c\n", array[2], array[4], array[6]);
	if ((array[0] == a && array [4] == a && array[8] == a) ||
	 (array[2] == a && array [4] == a && array[6] == a)){
		//printf("ENTROU DEPOIS\n");
		verificador = 1;
	}
	//printf("Verificador %d\n", verificador);	
	return verificador;
}

int verificaColunas(char array[9], char a){
	int verificador = 0;
	if ((array[0] == a && array[3] == a && array[6] == a) ||
	 (array[1] == a && array[4] == a && array[7] == a) ||
	  (array[2] == a && array[5] == a && array[8] == a)){
		verificador = 1;
	}
	return verificador;
}

int verificaLinhas(char array[9], char a) {
    int verificador = 0;
     
	if ((array[0] == a && array[1] == a && array[2] == a) ||
	 (array[3] == a && array[4] == a && array[5] == a) ||
	  (array[6] == a && array[7] == a && array[8] == a)){
		verificador = 1;
	}
    return verificador;

}

int verificaEmpate(char array[9]){
	int verifica = 2;
	for (int i = 0; i<9; i++){
		if (array[i] != 'X' && array[i] != 'O'){
			verifica = 0;
		}
	}

	return verifica;
}

int verificaVencedor(char array[9], char a){
	int verifica = 0;

	verifica = verificaColunas(array, a) + verificaDiagonais(array, a) + verificaLinhas(array, a);

	if (verifica > 0){
		verifica = 1;
	}
	else{
		verifica = verificaEmpate(array);
	}
	
	return verifica;

}


/*
* Esta função reporta erro e finaliza do programa:
*/
static void
bail(const char *on_what) {
	if ( errno != 0 ) {
		fputs(strerror(errno),stderr);
		fputs(": ",stderr);
	}
	fputs(on_what,stderr);
	fputc('\n',stderr);
	exit(1);
}




int main(int argc,char **argv) {
	int z;
	char *srvr_addr = NULL;
	srvr_addr = "127.0.0.1";
	char *srvr_port = "9999";
	struct sockaddr_in adr_srvr;/* AF_INET */
	struct sockaddr_in adr_clnt;/* AF_INET */
	socklen_t len_inet; /* comprimento  */
	int s; /* Socket do servidor */
	int c1; /* Socket par ao cliente */
	int c2;
	int n; /* nÃºmero de bytes lidos */
	struct mensagem jogo;
	char jogador1[20];
	char jogador2[20];

	
	


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
	if ( strcmp(srvr_addr,"*") != 0 ) {
		/* EndereÃ§o normal */
		adr_srvr.sin_addr.s_addr = inet_addr(srvr_addr);
		if ( adr_srvr.sin_addr.s_addr == INADDR_NONE )
			bail("bad address."); 
	} else {
		/* Qualquer endereÃ§o */
		adr_srvr.sin_addr.s_addr = INADDR_ANY;
	}

/*
* Liga (bind) o socket com o endereÃ§o/porta
*/
	len_inet = sizeof adr_srvr;
	z = bind(s,(struct sockaddr *)&adr_srvr, len_inet);
	if ( z == -1 )
		bail("bind(2)");

/*
* Coloca o socket do servidor em estado de "escuta"
*/
	z = listen(s,10);
	if ( z == -1 ){
		bail("listen(2)");
	}
	while(1){		
		printf("Aguardando Jogadores!!!\n");
		c1 = -1; 
		c2 = -1;
		while ( c1 == -1 || c2 == -1 )
		{
		/*
		* Aguarda por uma solitaÃ§Ã£o de conexÃ£o: cliente 1
		*/	
			if(c1 ==-1)
			{
				len_inet = sizeof adr_clnt;
				c1 = accept(s,(struct sockaddr *)&adr_clnt,&len_inet);

				if ( c1 == -1 )
					bail("accept(2)");
				
				//lê mensagem do cliente 1
				z = read(c1,&jogo,sizeof(struct mensagem));
					if ( z == -1 )
						bail("read(2)");

				strcpy(jogador1, jogo.nome);
			}
			else
			{	
				len_inet = sizeof adr_clnt;
				c2 = accept(s,(struct sockaddr *)&adr_clnt,&len_inet);

				if ( c2 == -1 )
					bail("accept(2)");
				
				//lê mensagem do cliente 2
				z = read(c2,&jogo,sizeof(struct mensagem));
				if ( z == -1 )
					bail("read(2)");
				
					strcpy(jogador2, jogo.nome);
				
			}
		

		}

		strcpy(jogo.msg, "Bem vindo ao jogo da velha!");

		jogo.tabuleiro[0] = '1';
		jogo.tabuleiro[1] = '2';
		jogo.tabuleiro[2] = '3';
		jogo.tabuleiro[3] = '4';
		jogo.tabuleiro[4] = '5';
		jogo.tabuleiro[5] = '6';
		jogo.tabuleiro[6] = '7';
		jogo.tabuleiro[7] = '8';
		jogo.tabuleiro[8] = '9';

		jogo.situacaoJogo = 0;

		strcpy(jogo.nome_adversario, jogador2);
		z = write(c1,(const void *) &jogo,sizeof(struct mensagem));
		if ( z == -1 )
			bail("write(2)");

		//Mensagem do cliente 2
		strcpy(jogo.nome_adversario, jogador1);
		z = write(c2,(const void *) &jogo,sizeof(struct mensagem));
		if ( z == -1 )
			bail("write(2)");


		
		while (1) {

			jogo.caractere = 'X';
			strcpy(jogo.nome, jogador1);
			strcpy(jogo.nome_adversario, jogador2);
			z = write(c1,(void *) &jogo,sizeof(jogo));
			z = read(c1,(void *) &jogo,sizeof(jogo));

			jogo.situacaoJogo = verificaVencedor(jogo.tabuleiro, jogo.caractere);
			//printf("SITUAÇÃO JOGO %d\n", jogo.situacaoJogo);

			if (jogo.situacaoJogo == 1){
				//printf("ENTROU VENCEDOR\n");
				z = write(c1,(void *) &jogo,sizeof(jogo));
				jogo.situacaoJogo = -1;

				strcpy(jogo.nome, jogador2);
				strcpy(jogo.nome_adversario, jogador1);
				z = write(c2,(void *) &jogo,sizeof(jogo));
				break;
			}

			if (jogo.situacaoJogo == 2){
				z = write(c1,(void *) &jogo,sizeof(jogo));

				strcpy(jogo.nome, jogador2);
				strcpy(jogo.nome_adversario, jogador1);
				z = write(c2,(void *) &jogo,sizeof(jogo));
				break;
			}

		
			// JOGADOR 2

			jogo.caractere = 'O';
			strcpy(jogo.nome, jogador2);
			strcpy(jogo.nome_adversario, jogador1);
			z = write(c2,(void *) &jogo,sizeof(jogo));
			z = read(c2,(void *) &jogo,sizeof(jogo));

			jogo.situacaoJogo = verificaVencedor(jogo.tabuleiro, jogo.caractere);

			if (jogo.situacaoJogo == 1){
				z = write(c2,(void *) &jogo,sizeof(jogo));
				jogo.situacaoJogo = -1;

				strcpy(jogo.nome, jogador1);
				strcpy(jogo.nome_adversario, jogador2);
				z = write(c1,(void *) &jogo,sizeof(jogo));
				break;
			}

			if (jogo.situacaoJogo == 2){
				z = write(c2,(void *) &jogo,sizeof(jogo));

				strcpy(jogo.nome, jogador1);
				strcpy(jogo.nome_adversario, jogador2);
				z = write(c1,(void *) &jogo,sizeof(jogo));
				break;
			}
		}
		printf("Partida Finalizada!\n");
		printf("Reiniciando......!\n");
		sleep(30);
		close(c1);
		close(c2);
	}

	/*z = write(c1, (void*) &jogo, sizeof(jogo));
	z = write(c2, (void*) &jogo, sizeof(jogo));*/

	//printf("SAIU\n");
	
	return 0;
}
