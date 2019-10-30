#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>

void drawHeader(){
    printf("-----------------------------\n");
    printf("- T H A N D E R - S H E L L -\n");
    printf("-----------------------------\n");
}

/* ler o que o usuário digita */
char readline[80];
/* Salva os parâmetros digitados */
char readparams[80];
/* Salva o programa a ser executado */
char cmd[80];
/* PATH com as pastas dos programas */
char path[6][256] = {"/usr/local/sbin","/usr/local/bin","/usr/sbin","/usr/bin","/sbin","/bin"};
/* Guarda o comando e PATH válidos */
char validPATH[80];
/* Pasta local do bash */
char cwd[256];
/* Guarda o historico de comandos */
char historyArray[29][30];
int conthistory=0;
/* Executa o History */
void history(){
    int x;
    if(conthistory > 0){
        for(x = 0; x < conthistory; x++){
            printf("%d %s\n",x, historyArray[x]);
        }
    }else{
        printf("Lista de comandos vazio!\n");
    }
}

/*Reseta todos os vetores de char */
void reset(){
    memset(readparams, 0, sizeof readparams);
    memset(cmd, 0, sizeof cmd);
    memset(readline, 0, sizeof readline);
    memset(validPATH, 0, sizeof validPATH);
}
/* Verifica o PWD */
char *pwd(){
   /* Verifica se o cwd é null */
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       return cwd;
   } else {
       perror("getcwd() error");
   }
   return 0;
}

/* Checa se o directório existe */
int checkDir(){
    DIR* dir = opendir(readparams);
    if (dir) {
        return 1;
    closedir(dir);
    }
    return 0;
}

/* Comando CD */
void cd(){
    if (checkDir())chdir(readparams);
    else printf("Directório inexistente!\n");
    if(!strcmp(readparams, ".."))chdir("..");
}

/*MKDIR*/
void call_mkdir(){
    if (checkDir())printf("Directório já existe!\n");
    else mkdir(readparams, 0777);  
}

/*RMDIR*/
void call_rmdir(){
    if (checkDir())rmdir(readparams);   
    else printf("Directório não existe!\n");
}

/*Checa se existe o comando no path */
int checkExe(){
    int i = 0;
    for(i = 0; i < 6; i++){
        /*Cria um char temporario */
        char tmp[80];
        /*copia o path para o temporario */
        strcpy(tmp, path[i]);
        /*acrescenta uma barra no final */
        strcat(tmp, "/");
        /*Concatena o path com o valor digitado */
        strcat(tmp, cmd);
        /* Faz a chamada do sistema access e verifica se existe */
        if (0 == access(tmp, 0)){
            /* Modifica a variavel validPATH com um programa que existe nas pastas do PATH */
            strcpy(validPATH, tmp);
            return 1;
        }
    } 
    return 0; 
}
/* Marcador para indicar quando o programa deve terminar */ 
int ComandoInterno(){
    int i = 0;
    while (i < 10)
    {
        if(!strcmp(cmd, "pwd")){
            printf("%s\n", pwd());
            reset();
            break;
        }else if(!strcmp(cmd, "exit")){
            exit(0);
            reset();
            break;
        }else if(!strcmp(cmd, "history")){
            history();
            reset();
            break;
        }else if(!strcmp(cmd, "echo")){
            printf("%s \n",readparams);
            reset();
            break;
        }else if(!strcmp(cmd, "cd")){
            cd();
            reset();
            break;
        }else if(!strcmp(cmd, "mkdir")){
            call_mkdir();
            reset();
            break;
        }else if(!strcmp(cmd, "rmdir")){
            call_rmdir();
            reset();
            break;
        }
        i++;
    }
    return 0;
}

void readCMD(){
    scanf(" %[^\n]s",readline);
    int i, j = 0;
    int flag = 1;
    for(i = 0; i < strlen(readline); i++){
         /* Compare strings */
        if(' ' == readline[i]){
            flag = 0;            
        }
        if(flag){
            cmd[i] = readline[i];
        }else if(' ' != readline[i]){
            readparams[j] = readline[i];
            j++;
        }
        
    }
    /*Incrementa os comandos ao history*/
    if(!strcmp(readparams, "-c")){
        memset(historyArray, 0, sizeof historyArray);
        conthistory = 0;
    }else{  
        /*Verifica se foi escrito a palavra history*/
        if(!strcmp(readline, "history")){
            
        }else{
            if(conthistory > 29){
                conthistory = 0;
            }
            strcpy(historyArray[conthistory], readline);
            conthistory++;
        }
    }  
    
}

int main(int argc, char** argv){
    int should_run = 1;
    drawHeader();
        while(should_run){
            /*Reset char*/
            reset();
            printf("root@ThanderShell:%s# ", pwd());
            fflush(stdout);    
            readCMD();
            if(ComandoInterno()){
                /* comando interno */
            }else{
                pid_t p = fork();
                if(p<0){
                    printf("Erro ao criar filho");
                }else{
                    if(p){
                        wait(NULL);
                    }else{            
                        if(checkExe()){
                            if(strlen(readparams) > 0){ 
                                execl(validPATH, cmd, readparams, NULL);
                            }else{
                                execl(validPATH, cmd, 0, NULL);
                            }
                        }else{
                            printf("O comando {%s} é inexistente!\n", cmd);
                        }                    
                }   
            }
        }     
    }
    return 0;
}