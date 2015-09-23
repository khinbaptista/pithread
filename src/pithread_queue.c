#include "pithread_queue.h"


TCB_t* NextThread(TCB_t* queue){
	TCB_t* next;
	if (queue == NULL)
		return NULL;
	
	next = queue->next;
	
	return next;
}

TCB_t* AddThread(TCB_t* queue, TCB_t* thread){
	if (queue == NULL){
		queue = thread;
		thread->next = NULL;
		thread->prev = NULL;
		
		return queue;
	}
	
	/* New thread has more credits than first element */
	if (thread->credReal > queue->credReal){
		printf("MAIOR %d\n", thread->tid);
		thread->next = queue;
		queue->prev = thread;
		thread->prev = NULL;
		
		queue = thread;
		
		return queue;
	}
	
	
	/* Insert new thread in position */
	
	// iterator
	TCB_t* it = queue;
	
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

	return queue;
}

TCB_t* AddToMutex(TCB_t* mutexQueue, TCB_t* thread){
	if (mutexQueue == NULL){
		thread->next = NULL;
		thread->prev = NULL;
		mutexQueue = thread;
		
		return mutexQueue;
	}
	
	/* Insert new thread in position */
	
	// iterator
	TCB_t* it = mutexQueue;
	
	while (it->next != NULL){
		it = it->next;
	}
	// position found
	it->next = thread;
	thread->prev = it;
	thread->next = NULL;

	return mutexQueue;
}

TCB_t* RemoveFromMutex(TCB_t* mutexQueue){
	if (mutexQueue){
		mutexQueue = mutexQueue->next;
		mutexQueue->prev = NULL;
	}
	return mutexQueue;
	
}

void SwapQueues(TCB_t** a, TCB_t** b){
	TCB_t* c = *a;
	*a = *b;
	*b = c;
}

TCB_t* GetThread(TCB_t* queue, int tid){
	TCB_t* it = queue;
	
	while (it != NULL && it->tid != tid)
		it = it->next;
		
	return it;
}

TCB_t* RestoreCredits(TCB_t* queue){
	TCB_t* it = queue;
	
	while (it != NULL){
		it->credReal = it->credCreate;
		it = it->next;
	}
	return queue;
}

TCB_t* RemoveThread(TCB_t* queue, int tid){
	TCB_t* it = queue;
	
	while (it != NULL && it->tid != tid)
		it = it->next;
	
	if(it){
		if(it->prev)
			it->prev->next = it->next;
		if(it->next)
			it->next->prev = it->prev;
	}

	return queue;
}

WaitQueue_t* GetWait(WaitQueue_t* queue, int tid){
	WaitQueue_t* it = queue;
	
	while (it != NULL && it->tid != tid)
		it = it->next;
		
	return it;
}

WaitQueue_t* RemoveWait(WaitQueue_t* queue, int tid){
	WaitQueue_t* it = queue;
	WaitQueue_t* removed;

	if(it != NULL && it->tid != tid){
		while (it->next != NULL && it->next->tid != tid)
			it = it->next;
	}
	if(it->next){
		removed = it->next;
		it->next = it->next->next;
		removed->next = NULL;
		free(removed);
	}
		
	return queue;

}

WaitQueue_t* AddWait(WaitQueue_t* queue, int tid, int waiting){
	WaitQueue_t* node;
	WaitQueue_t* it = queue;

	if( ( node = (WaitQueue_t*)malloc(sizeof(WaitQueue_t)) ) ){
		node->tid = tid;
		node->waiting = waiting;
		node->next = NULL;
		
		if(queue == NULL){
			queue = node;
		}
		else{
			while (it->next != NULL)
				it = it->next;
			it->next = node;
		}
	
	}
	return queue;

}
