#include "pidata.h"
#include "pithread.h"
#include "pithread_queue.h"

TCB_t* activeThreads;
TCB_t* expiredThreads;
TCB_t* blockedThreads;

TCB_t* runningThread;

int counter;
int initialized;

// Helper functions signatures

void Initialize(void);
int inline CheckInit();
void DecreaseCredits(TCB_t* t);


// Implementations

void Initialize(){
	if (initialized == 1) return;
	
	initialized = 1;
	counter = 0;
}

int picreate(int cred, void* (*entry)(void*), void *arg){
	Initialize();
	
	//TCB_t* thread = (TCB_t*)malloc(sizeof(TCB_t));
	//thread->
	
	return 1;
}

int piyield(){
	if (!CheckInit()) return 0;
	
	DecreaseCredits(runningThread);
	
	if (runningThread->credReal == 0)
		AddThread(expiredThreads, runningThread);
	else
		AddThread(activeThreads, runningThread);
	
	runningThread->state = ABLE;
	
	runningThread = NextThread(activeThreads);
	
	if (runningThread == NULL){
		SwapQueues(activeThreads, expiredThreads);
		runningThread = NextThread(activeThreads);
		
		// No more threads to run, exit process
		if (runningThread == NULL)
			exit(0);
	}
	
	if (runningThread)
		runningThread->state = EXECUTION;
		
	return 1;
}


// Helper functions implementations

int inline CheckInit(){
	if (initialized != 1) return 0;
	return 1;
}

void DecreaseCredits(TCB_t* t){
	int credits = t->credReal;
	credits -= EXECUTION_COST;
	
	if (credits < 0) credits = 0;
	
	t->credReal = credits;
}
