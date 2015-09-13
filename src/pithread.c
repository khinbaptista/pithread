#include <ucontext.h>
#include "pidata.h"
#include "pithread.h"
#include "pithread_queue.h"

TCB_t* activeThreads;
TCB_t* expiredThreads;
TCB_t* blockedThreads;

TCB_t* runningThread;
u_context finishCtx;

int counter;
int initialized;

int executeThread;

// Helper functions signatures

void Initialize(void);
int inline CheckInit();
void DecreaseCredits(TCB_t* t);
void schedule();
void finishThread();


// Implementations

void Initialize(){
	if (initialized == 1) return;

	u_context mainCtx;
	char mainStack[SIGSTKSZ];
	char finishStack[SIGSTKSZ];
	

	initialized = 1;
	counter = 0;

	executeThread = 0;

	//FINISH THREAD CONTEXT CREATION
	getcontext(&finishCtx);
	finishCtx.uc_link = NULL;
	finishCtx.uc_stack.ss_sp = finishStack;
	finishCtx.uc_stack.ss_size = sizeof(finishStack);

	makecontext(&finishCtx, (void (*)(void)) finishThread, 0, void);


	//MAIN THREAD CREATION
	TCB_t* mainThread = (TCB_t*)malloc(sizeof(TCB_t));

	mainThread->tid = counter++;
	mainThread->state = EXECUTION;
	mainThread->credCreate = 0;
	mainThread->credReal = 0;

	getcontext(&mainCtx);

	mainCtx.uc_link = NULL;
	mainCtx.uc_stack.ss_sp = mainStack;
	mainCtx.uc_stack.ss_size = sizeof(mainStack);

	mainThread->context = mainCtx;

	runningThread = mainThread;
}

int picreate(int cred, void* (*entry)(void*), void *arg){
	Initialize();
	char newStack[SIGSTKSZ];
	ucontext_t newContext;


	if(TCB_t* thread = (TCB_t*)malloc(sizeof(TCB_t))){
		// NEW THREAD CREATION
		thread->tid = counter++;
		thread->state = ABLE;	// precisa ter um state "CREATION"?
		thread->credCreate = cred;
		thread->credReal = cred;

		getcontext(&newContext);
		newContext.uc_link = finishCtx;
;
		newcontext.uc_stack.ss_sp = newStack;
		newContext.uc_stack.ss_size = sizeof(newStack);

		
		makecontext(&newContext, (void (*)(void)) entry, 1, arg);

		

		thread->context = newContext;

		AddThread(activeThreads, thread);

		return 0;
	}

	return -1;
}

int piyield(){
	//TCB_t* oldThread;

	if (!CheckInit()) return -1;

	
	/*
	DecreaseCredits(runningThread);

	if (runningThread->credReal == 0)
		AddThread(expiredThreads, runningThread);
	else
		AddThread(activeThreads, runningThread);

	runningThread->state = ABLE;
	oldThread = runningThread;
	
	runningThread = NextThread(activeThreads);

	if (runningThread == NULL){
		SwapQueues(activeThreads, expiredThreads);
		runningThread = NextThread(activeThreads);
	}
	else{
		runningThread->state = EXECUTION;
	}*/

	schedule();

	return swapcontext(oldThread->context, runningThread->context);
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

// WHEN THREAD FINISHES
void finishThread(){
	runningThread->state = FINISHED;
	schedule();
	setcontext(runningThread->context);
}

// SELECTS NEXT RUNNING THREAD
void schedule(){
	TCB_t* oldThread;

	// IF RUNNING THREAD IS NOT FINISHED
	// PUTS THE THREAD IN ONE OF THE TWO
	// ABLE QUEUES
	if(runningThread->state != FINISHED){
		DecreaseCredits(runningThread);

		if (runningThread->credReal == 0)
			AddThread(expiredThreads, runningThread);
		else
			AddThread(activeThreads, runningThread);
		
		runningThread->state = ABLE;
		oldThread = runningThread;

	}
	// IF THE THREAD IS FINISHED, 
	// FREE THE TCB MEMORY
	else{
		free(runningThread);	
	}

	// SELECTS NEXT THREAD TO RUN	
	runningThread = NextThread(activeThreads);

	if (runningThread == NULL){
		SwapQueues(activeThreads, expiredThreads);
		runningThread = NextThread(activeThreads);
	}
	

	runningThread->state = EXECUTION;
	
}
