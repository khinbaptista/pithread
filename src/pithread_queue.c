#include "pithread_queue.h"

TCB_t* NextThread(TCB_t** queue){
	if (queue == NULL)
		return NULL;
	
	TCB_t* next = *queue;
	*queue = (*queue)->next;
	
	return next;
}

void AddThread(TCB_t** queue, TCB_t* thread){
	if (queue == NULL){
		*queue = thread;
		thread->next = NULL;
		thread->prev = NULL;
		
		return;
	}
	
	/* New thread has more credits than first element */
	if (thread->credReal > (*queue)->credReal){
		thread->next = *queue;
		(*queue)->prev = thread;
		thread->prev = NULL;
		
		*queue = thread;
		
		return;
	}
	
	
	/* Insert new thread in position */
	
	// iterator
	TCB_t* it = *queue;
	
	do{		// position found
		if (thread->credReal > it->credReal){
			thread->next = it;
			thread->prev = it->prev;
			it->prev = thread;
			thread->prev->next = thread;
			
			it = NULL;
		}	// end of queue
		else if (it->next == NULL){
			it->next = thread;
			thread->prev = it;
			thread->next = NULL;
			
			it = NULL;
		}
		else	// keep looking
			it = it->next;
	}
	while (it != NULL);
}

void SwapQueues(TCB_t** a, TCB_t** b){
	TCB_t** c = a;
	a = b;
	b = c;
}

TCB_t* GetThread(TCB_t** queue, int tid){
	TCB_t* it = *queue;
	
	while (it != NULL && it->tid != tid)
		it = it->next;
		
	return it;
}
