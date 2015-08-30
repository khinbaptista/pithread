/*
 * pithread.h: arquivo de inclusão com os protótipos das funções a serem
 *             implementadas na realização do trabalho.
 *
 * NÃO MODIFIQUE ESTE ARQUIVO.
 *
 * VERSÃO 1 - 17/08/2015
 */
#ifndef __pithread__
#define __pithread__

typedef struct mutex {
	int	flag; 			// indica se o mutex foi adquirido ou não (1: livre; 0: ocupado)
	struct TCB	*first, *last; 	// ponteiros para lista de threads bloqueadas no mutex
} pimutex_t;

int picreate (int credCreate, void* (*start)(void*), void *arg);
int piyield(void);
int piwait(int tid);
int pimutex_init(pimutex_t *mtx);
int pilock (pimutex_t *mtx);
int piunlock (pimutex_t *mtx);

#endif
