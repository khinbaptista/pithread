#include "pidata.h"
#include "pithread.h"
#include "pithread_op.h"
#include "pithread_queue.h"

TCB_t* activeThreads;
TCB_t* expiredThreads;
TCB_t* blockedThreads;

TCB_t* runningThread;


// Helper functions signatures

void DecreaseCredits(TCB_t* t);


// Implementations

void piyield(){
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
}


// Helper functions implementations

void DecreaseCredits(TCB_t* t){
	int credits = t->credReal;
	credits -= 10;
	
	if (credits < 0) credits = 0;
	
	t->credReal = credits;
}
