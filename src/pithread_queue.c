#include "pithread_queue.h"

TCB_t* NextThread(TCB_t* queue){
	if (queue == NULL)
		return NULL;
	
	TCB_t* next = queue;
	queue = queue->next;
	
	return next;
}

void AddThread(TCB_t* queue, TCB_t* thread){
	if (queue == NULL){
		queue = thread;
		thread->next = NULL;
		thread->prev = NULL;
		
		return;
	}
	
	
}

void SwapQueues(TCB_t* a, TCB_t* b){
	TCB_t* c = a;
	a = b;
	b = c;
}
