// www.trsampaio.com       //
// Tiago Roberti Sampaio  //
////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>

//----------Variáveis Globais---------
pid_t pid;
char cmd[50]; //armazena comando digitado pelo usuário
char param[50];
char program_name[50];
char path[50];
int i = 0; //percorre a string acima
int p = 0;
int param_flag = 0;
//----------Protótipos--------------
void antizumbi(int sig);

//---------BODY------------
int main () {
	signal(SIGCHLD, antizumbi); //Vigia anti-zumbi
	
	printf("------------------Mini-Shell--------------------\n");
	printf("Instrucoes:\n");
	printf("Simplesmente digite o comando e aperte enter :-)\n");
	printf("Obs: exit para sair\n");
	printf("------------------------------------------------\n");

	while(1) { //Condição de saida: cmd == exit		
		printf ("usr(%d)> ",pid);
		gets(cmd);

		//////////////// Separa o nome do programa e os paramêtros ////////////////
		while ( cmd[i] != '\0' ) {
			if ( param_flag == 0 && cmd[i] == ' ' ) { //Condição para armazenar o nome do programa e separar dos parâmetros
				program_name[p] = '\0';
				//printf ("c: %c\n", cmd[i]);
				//printf ("c: %c\n", program_name[p]);
				p = 0;
				i++;
				if ( cmd[i] == '&' ) break;
				param_flag = 1;				
				continue;
			}
			if ( param_flag ) {
				param[p++] = cmd[i];
			} else {
				program_name[p++] = cmd[i];
			}	
			i++;
		}
		param[p] = '\0';
		if (param_flag == 0) {
			//printf("nao tem parametros\n");
			strcpy(program_name,cmd);
			strcpy(param," ");
		}
		/////////////////////////////////////////////////////////////
		////////PEGA PATH////////
			char busca[50];
			strcpy(busca, "which ");
			strcat(busca, program_name);
			if( strcmp(cmd, "exit") == 0 ) break;
			FILE * f = popen(busca,"r");
			char path[50];
			fgets(path, 50, f);fclose(f);
			//printf("peguei: %s\n", path);
			if ( strlen(path) == 0 ) { 
				printf("Comando (%s) nao existente! Tente novamente.\n",program_name);
				i = 0; p = 0; param_flag = 0; strcpy(param,""); strcpy(cmd,""); strcpy(program_name,""); strcpy(busca,""); strcpy(path,"");
				continue;
			}
			path[strlen(path)-1] = '\0'; //remove o \n no final do comando
		//////////////////////

		/////////////////Execução com &, o pai não espera filho terminar de executar////////////////////////
		if ( param[strlen(param)-1] == '&' || program_name[strlen(program_name)-1] == '&' ) {
			printf ("\tAcao &\n");
			param[strlen(param)-1] = '\0'; //Limpando o sinalizador "&"
			char* lista_param[] = { path, param, NULL };

			//Hora de duplicar!
			pid = fork();		
			if ( pid < 0 ) {
				printf("Processo falhou\n");
			} else if ( pid == 0 ) { //processo filho
				//printf("executar %s|%s|\n",lista_param[0],lista_param[1]);
				execvp(lista_param[0],lista_param);return; //Filho faz o que tem que fazer e sai
			} else { //processo pai
				printf("!!!CMD!!! - %s executando com PID n%d\n",program_name,pid);
				sleep(2); //Dá um tempo para aparecer os resultado do filho
			}
		/////////////////Execução simples, o pai espera filho terminar de executar////////////////////////			
		} else {
			printf ("\tExecucao simples\n");
			
			char* lista_param[] = { path, param, NULL };
			//Hora de duplicar!
			pid = fork();						
			if ( pid < 0 ) {
				printf("Processo falhou\n");
			} else if ( pid == 0 ) { //processo filho
				//printf("executar %s|%s|\n",lista_param[0],lista_param[1]);
				execvp(lista_param[0],lista_param);return; //Filho faz o que tem que fazer e sai					
			} else { //processo pai
				printf("!!!CMD!!! - Esperando execucao do processo n%d\n",pid);
				waitpid(pid, NULL, 0);
			}
		}
		//Limpeza de variáveis
		i = 0; p = 0; param_flag = 0; strcpy(param,""); strcpy(cmd,""); strcpy(program_name,""); strcpy(busca,""); strcpy(path,"");
	}

	return 0;
}


void antizumbi(int sig) {
	//printf ("Evitando zumbis ...  possivel zumbi(%d)\n",pid);
	waitpid(pid, NULL, 0);
	//printf ("Zumbi evitado! SIG: %d\n",sig);

}