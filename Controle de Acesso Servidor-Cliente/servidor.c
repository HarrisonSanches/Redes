/* server.c:
*
* Exemplo de servidor de data e hora:
*/
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

typedef struct {
	int porta; //porta acessar
	int codigo; //codigo
}requisitoAcesso;

typedef struct {
    char nome[50]; //nome do usuario
    char autorizacao; //porta garantido ou não
}respostaAcesso;

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
	char *srvr_port = "9099";
	struct sockaddr_in adr_srvr;/* AF_INET */
	struct sockaddr_in adr_clnt;/* AF_INET */
	int len_inet; /* comprimento  */
	int s; /* Socket do servidor */
	int c; /* Socket par ao cliente */

	int n; /* número de bytes lidos */	
	time_t td; /* Data e hora atual */
	char dtbuf[128]; /* Informações de data e hora */

	int codigoCredenciais;//CODIGO DE ACESSO
	int nivelAcessoCredenciais;//NIVEL QUE PODE ACESSAR
	char nomeCredenciais[50];//NOME DO USUARIO
	char linha[100];//LINHA LIDA DO ARQUIVO
	char* token;//LINHA DIVIDIDA
	int achou = 0;//ENCONTROU A PESSOA

    FILE* credenciais;//ARQUIVO CREDENCIAIS
    FILE* registro;//ARQUIVO REGISTRO
	

	requisitoAcesso msg_cliente;
    respostaAcesso msg_servidor;

	credenciais = fopen("credenciais.txt", "r");
	registro = fopen("registro.txt", "a");
	//fprintf(registro, "TESTE\n");
	
	

	if ( argc >= 2 ) {
		/* Verifica se o endereço do servidor foi passado como argumento	 */
		srvr_addr = argv[1];
	} else {
		/* Usa o valor padrão	 */
		srvr_addr = "127.0.0.1";
	}

/*
* Verifica se o numero da porta foi passado como argumento
*/
	if ( argc >= 3 )
		srvr_port = argv[2];

/*
* Cria um socket do tipo TCP
*/
	s = socket(PF_INET,SOCK_STREAM,0);
	if ( s == -1 )
		bail("socket()");

/*
 * Preenche a estrutura do socket com a porta e endereço do servidor
 */
	memset(&adr_srvr,0,sizeof adr_srvr);
	adr_srvr.sin_family = AF_INET;
	adr_srvr.sin_port = htons(atoi(srvr_port));
	if ( strcmp(srvr_addr,"*") != 0 ) {
		/* Endereço normal */
		adr_srvr.sin_addr.s_addr = inet_addr(srvr_addr);
		if ( adr_srvr.sin_addr.s_addr == INADDR_NONE )
			bail("bad address."); 
	} else {
		/* Qualquer endereço */
		adr_srvr.sin_addr.s_addr = INADDR_ANY;
	}

/*
* Liga (bind) o socket com o endereço/porta
*/
	len_inet = sizeof adr_srvr;
	z = bind(s,(struct sockaddr *)&adr_srvr, len_inet);
	if ( z == -1 )
		bail("bind(2)");

/*
* Coloca o socket do servidor em estado de "escuta"
*/
	z = listen(s,10);
	if ( z == -1 )
		bail("listen(2)");

/*
* Inicia o loop do servido:
*/
	for (;;) {
		/*
		* Aguarda por uma solitação de conexão:
		*/
			len_inet = sizeof adr_clnt;
			c = accept(s,(struct sockaddr *)&adr_clnt,&len_inet);

			if ( c == -1 )
				bail("accept(2)");

		/*
		* Gera a data e hora atual:
		*/

        //LÊ MENSAGEM DO CLIENTE
        z = read(c,&msg_cliente,sizeof(requisitoAcesso));
	    if ( z == -1 ){
		    bail("read(2)");
        }

		time(&td);
		n = (int) strftime(dtbuf,sizeof dtbuf, "%d/%m/%Y - %H:%M:%S", localtime(&td));
		//04/09/2018 – 14:11:35, p2, 7458, negado
		dtbuf[n] = 0;
		
        
		/*printf("CÓDIGO %d\n", msg_cliente.codigo);
        printf("PORTA %d\n", msg_cliente.porta);

		msg_servidor.autorizacao = 'A';

        strcpy(msg_servidor.nome, "HULK SMASH");*/
        

        //printf("MSG AUTORIZACAO %s \n", msg_servidor.autorizacao);

		msg_servidor.autorizacao = 'N';
        strcpy(msg_servidor.nome, "Usuário desconhecido");
        

        //VERIFICA CREDENCIAIS
		achou = 0;		
		while ((fgets(linha, 98, credenciais) != NULL) && achou == 0){
			token = strtok(linha, ",");
			
			codigoCredenciais = atoi(token);
			token = strtok(NULL, ",");
			strcpy(nomeCredenciais, token);
			token = strtok(NULL, ",");
			nivelAcessoCredenciais = atoi(token);
				
			/*printf("CODIGO %d\n", codigoCredenciais);
			printf("NOME %s\n", nomeCredenciais);
			printf("NIVEL %d\n", nivelAcessoCredenciais);*/

			if (msg_cliente.codigo == codigoCredenciais){
				strcpy(msg_servidor.nome, nomeCredenciais);
				//printf("NOME CREDENCIAL COPIADO%s\n", nomeCredenciais);
				if (msg_cliente.porta <= nivelAcessoCredenciais){
					msg_servidor.autorizacao = 'A';
					//printf("AUTORIZACAO COPIADO%d\n", nivelAcessoCredenciais);
				}
				achou = 1;
			}
		}
		rewind(credenciais);

		//04/09/2018 – 14:11:35, p2, 7458, negado
		if (msg_servidor.autorizacao == 'A'){
			//printf("%s, %d, %d, Autorizado\n", dtbuf, msg_cliente.porta, msg_cliente.codigo);
			fprintf(registro, "%s, %d, %d, Autorizado\n", dtbuf, msg_cliente.porta, msg_cliente.codigo);
			//fprintf(registro, "teste1\n");
			//printf("%s", dtbuf);
		}
		else{
			//printf("%s, %d, %d, Negado\n", dtbuf, msg_cliente.porta, msg_cliente.codigo);
			fprintf(registro, "%s, %d, %d, Negado\n", dtbuf, msg_cliente.porta, msg_cliente.codigo);
			//fprintf(registro, "teste2\n");
			//printf("%s\n", dtbuf);
		}



        z = write(c,(const void *) &msg_servidor,sizeof(respostaAcesso));

		if ( z == -1 )
			bail("write(2)");
		
		close(c);
		fclose(registro);
		registro = fopen("registro.txt", "a");
	}

	fclose(credenciais);
	//fclose(registro);
	return 0;
}
