#include <ucontext.h>
#include "pidata.h"
#include "pithread.h"
#include "pithread_queue.h"

TCB_t* activeThreads;
TCB_t* expiredThreads;
TCB_t* blockedThreads;

TCB_t* runningThread;

int counter;
int initialized;

int executeThread;

// Helper functions signatures

void Initialize(void);
int inline CheckInit();
void DecreaseCredits(TCB_t* t);


// Implementations

void Initialize(){
	if (initialized == 1) return;

	initialized = 1;
	counter = 0;

	executeThread = 0;

	TCB_t* mainThread = (TCB_t*)malloc(sizeof(TCB_t));

	mainThread->tid = counter++;
	mainThread->state = EXECUTION;
	mainThread->credCreate = 0;
	mainThread->credReal = 0;
	getcontext(&mainThread->context);

	runningThread = mainThread;
}

int picreate(int cred, void* (*entry)(void*), void *arg){
	Initialize();
	char newStack[SIGSTKSZ];
	ucontext_t newContext;


	if(TCB_t* thread = (TCB_t*)malloc(sizeof(TCB_t))){
		thread->tid = counter++;
		thread->state = ABLE;	// precisa ter um state "CREATION"?
		thread->credCreate = cred;
		thread->credReal = cred;

		getcontext(&newContext);
		newContext.uc_link = NULL;
		newcontext.uc_stack.ss_sp = newStack;
		newContext.uc_stack.ss_size = sizeof(newStack);

		//CORRIGIR ESSE MAKECONTEXT
		makecontext(&newContext, (void (*)(void)) entry, 2, arg);

		thread->context = newContext;

		AddThread(activeThreads, thread);

		return 0;
	}

	return -1;
}

int piyield(){
	TCB_t* oldThread;

	if (!CheckInit()) return 0;

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
	}

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
