#include "pithread_queue.h"

TCB_t* NextThread(TCB_t* queue){
	if (queue == NULL)
		return NULL;
	
	TCB_t* next = queue;
	queue = queue->next;
	
	return next;
}

void SwapQueues(TCB_t* a, TCB_t b){
	TCB_t* c = a;
	a = b;
	b = c;
}
