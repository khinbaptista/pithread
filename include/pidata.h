/*
 * pidata.h: arquivo de inclusão de uso apenas na geração da libpithread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida.
 *
 */

#ifndef __pidata__
#define __pidata__

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// Thread state codes
#define CREATION 0
#define ABLE 1
#define EXECUTION 2
#define BLOCKED 3
#define FINISHED 4

// Cost for a thread to execute
#define EXECUTION_COST 10

/* NÃO ALTERAR ESSA struct */
typedef struct TCB {
	int	tid; 			// identificador da thread
	int	state;			// estado em que a thread se encontra
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
	int	credCreate;		// créditos de criação da thread (atribuídos na criação)
	int	credReal; 		// créditos atuais da thread (usado para determinar a prioridade atual da thread)
	ucontext_t 	context;	// contexto de execução da thread (SP, PC, GPRs e recursos)
	struct TCB 	*prev; 		// ponteiro para o TCB anterior da lista
	struct TCB 	*next; 		// ponteiro para o próximo TCB da lista
} TCB_t;

#endif
