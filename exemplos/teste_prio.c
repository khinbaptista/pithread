/* 
 * test_vetor.c: realiza a criação de 10 threads, cada uma delas escreve uma
 * sequencia de 20 letras iguais e passa a vez para outra thread. Repete até
 * preencher um vetor de 250 caracteres.
 *
 * A diferença em relação a anterior que é a primeira, quarta, setima e a décima
 * thread são criadas com a prioridade 0 (alta), a segunda, quinta, oitava thread
 * com prioridade 1 (média) e a terceira, sexta e nona thread com prioridade 2 
 * (baixa). Isso altera a saída do programa em questão.
 */

#include	"../include/pithread.h"
#include	<stdio.h>
#include	<stdlib.h>

#define		MAX_SIZE	250
#define		MAX_THR		10

int vetor[MAX_SIZE];
int  inc = 0;

void *func(void *arg){

   while ( inc < MAX_SIZE ) {
       vetor[inc] = (int)arg;
       inc++;
       if ( (inc % 20) == 0 )
           syield();
       else
           continue;
   }

   return (NULL);
}


int main(int argc, char *argv[]) {
    int i, pid[MAX_THR];

  
    for (i = 0; i < MAX_THR; i++) {
        pid[i] = picreate((i%3)+1, func, (void *)('A'+i));
       if ( pid[i] == -1) {
          printf("ERRO: criação de thread!\n");
          exit(-1);
       }
     }

    for (i = 0; i < MAX_THR; i++) 
         piwait(pid[i]);

    for (i = 0; i < MAX_SIZE; i++) {    
        if ( (i % 20) == 0 )
           printf("\n");
        printf("%c", (char)vetor[i]);
    }
      
    printf("\nConcluido vetor de letras com priuoridades...\n");
    exit(0);
}

