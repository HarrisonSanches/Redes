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
 #include <string.h>

typedef struct {
	int porta; //porta acessar
	int codigo; //codigo
}requisitoAcesso;

typedef struct {
    char nome[50]; //nome do usuario
    char autorizacao; //porta garantido ou não
}respostaAcesso;

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

 int main(int argc,char *argv[]) {	
	 int z;//socket do servidor
     int x;
	 char *srvr_addr = NULL;
	 char *srvr_port = "9099";
	 struct sockaddr_in adr_srvr;/* AF_INET */
	 int len_inet; /* comprimento */
	 int s; /* Socket */

     respostaAcesso msg_recebe; //MENSAGEM QUE CLIENTE RECEBE
     int usuario; // CODIGO DO USUARIO
     requisitoAcesso msg_envio; // MENSAGEM QUE CLIENTE ENVIA

     
     printf("Por favor insira o código de usuário:\n");
     scanf("%d" , &usuario);
     msg_envio.codigo = usuario;

     msg_envio.porta = atoi(argv[1]);
		
	/* Usa o valor padrão	 */
	srvr_addr = "127.0.0.1";		
			
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
	 adr_srvr.sin_addr.s_addr =
	 inet_addr(srvr_addr);
	 if ( adr_srvr.sin_addr.s_addr == INADDR_NONE )
		 bail("bad address.");

	 /*
	 * Conecta com o servidor
	 */
	 len_inet = sizeof adr_srvr;

	 z = connect(s,(struct sockaddr *) &adr_srvr,len_inet);
	 if ( z == -1 )
		 bail("connect(2)");

     

     //Envia parametros de porta
     x = write(s,(const void *) &msg_envio,sizeof(requisitoAcesso));
	 //printf("x: %d\n", x);

     

	 //Recebe mensagem
	 z = read(s,&msg_recebe, sizeof(respostaAcesso));
	 if ( z == -1 )
		 bail("read(2)");

     //imprime o porta
	 //printf("AUTORIZACAO %c\n", msg_recebe.autorizacao);
	 if (msg_recebe.autorizacao == 'A'){
         printf("Acesso autorizado. Bem-vindo %s", msg_recebe.nome);
     }
     else{
         printf("Acesso negado. %s suas credencias não permitem porta a esta sala", msg_recebe.nome);
     }
	 
		
	 /*	
	 * Fecha o socket	
	 */	
	 close(s);	
	 putchar('\n');	
		
	 return 0;	
 }	