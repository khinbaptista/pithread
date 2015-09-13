#include <ucontext.h>
#include "../include/pidata.h"
#include "../include/pithread.h"
#include "../include/pithread_queue.h"

TCB_t* activeThreads;
TCB_t* expiredThreads;

TCB_t* blockedActiveThreads;
TCB_t* blockedExpiredThreads;
TCB_t* mtxBlockedThreads;

TCB_t* waitedThreads;

TCB_t* runningThread;


ucontext_t finishCtx;


int counter;
int initialized;

// Helper functions signatures

void Initialize(void);
int inline CheckInit();
void DecreaseCredits(TCB_t* t);
void schedule();
void finishThread();
void unblock(int tid);


// Implementations

void Initialize(){
	if (initialized == 1) return;

	ucontext_t mainCtx;
	char mainStack[SIGSTKSZ];
	char finishStack[SIGSTKSZ];
	

	initialized = 1;
	counter = 0;

	//FINISH THREAD CONTEXT CREATION
	getcontext(&finishCtx);
	finishCtx.uc_link = NULL;
	finishCtx.uc_stack.ss_sp = finishStack;
	finishCtx.uc_stack.ss_size = sizeof(finishStack);

	makecontext(&finishCtx, (void (*)(void)) finishThread, 0, NULL);


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
	TCB_t* thread;

	if( ( thread = (TCB_t*)malloc(sizeof(TCB_t)) ) ){
		// NEW THREAD CREATION
		thread->tid = counter++;
		thread->state = ABLE;	// precisa ter um state "CREATION"?
		thread->credCreate = cred;
		thread->credReal = cred;

		getcontext(&newContext);
		newContext.uc_link = &finishCtx;

		newContext.uc_stack.ss_sp = newStack;
		newContext.uc_stack.ss_size = sizeof(newStack);

		
		makecontext(&newContext, (void (*)(void)) entry, 1, arg);

		

		thread->context = newContext;

		AddThread(activeThreads, thread);

		return 0;
	}

	return -1;
}

int piwait(int tid){
	TCB_t* waitedThread;
	
	char schedulerStack[SIGSTKSZ];
	ucontext_t schedulerContext;
	
	char unblockStack[SIGSTKSZ];
	ucontext_t unblockContext;


	waitedThread = GetThread(activeThreads, tid);
	if(!waitedThread){
		waitedThread = GetThread(expiredThreads, tid);
		if(!waitedThread){
			waitedThread = GetThread(blockedActiveThreads, tid);
			if(!waitedThread){
				waitedThread = GetThread(blockedExpiredThreads, tid);			
				if(!waitedThread){
					waitedThread = GetThread(mtxBlockedThreads, tid);			
				}
			}
		}	
	}
	
	if(waitedThread){
		if(!GetThread(waitedThreads, tid)){
			runningThread->state = BLOCKED;
			
			//SCHEDULER CONTEXT CREATION
			getcontext(&schedulerContext);
			schedulerContext.uc_link = NULL;
			schedulerContext.uc_stack.ss_sp = schedulerStack;
			schedulerContext.uc_stack.ss_size = sizeof(schedulerStack);

			makecontext(&schedulerContext, (void (*)(void)) schedule, 0, NULL);
			
			//UNBLOCK CONTEXT CREATION
			getcontext(&unblockContext);
			unblockContext.uc_link = NULL;
			unblockContext.uc_stack.ss_sp = unblockStack;
			unblockContext.uc_stack.ss_size = sizeof(unblockStack);

			makecontext(&unblockContext, (void (*)(void)) schedule, 1, runningThread->tid);
			

			waitedThread->context.uc_link = &unblockContext;

			swapcontext(&runningThread->context, &schedulerContext);

			return 0;
		}
	}

	return -1;

}




int piyield(){
	TCB_t* oldThread;

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
	runningThread->state = ABLE;
	oldThread = runningThread;
	
	schedule();

	return swapcontext(&oldThread->context, &runningThread->context);
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
	setcontext(&runningThread->context);
}

// SELECTS NEXT RUNNING THREAD
void schedule(){

	// IF RUNNING THREAD IS NOT FINISHED
	// PUTS THE THREAD IN ONE OF THE TWO
	// ABLE QUEUES
	if(runningThread->state != FINISHED){
		DecreaseCredits(runningThread);

		if (runningThread->credReal == 0){
			if(runningThread->state == ABLE){	
				AddThread(expiredThreads, runningThread);
			}
			if(runningThread->state == BLOCKED){	
				AddThread(blockedExpiredThreads, runningThread);
			}
		}
		else{
			if(runningThread->state == ABLE){
				AddThread(blockedActiveThreads, runningThread);
			}
			if(runningThread->state == BLOCKED){
				AddThread(blockedExpiredThreads, runningThread);
			}
		}		

		

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

void unblock(int tid){
	TCB_t* blockedThread;

	blockedThread = GetThread(blockedActiveThreads, tid);

	if(!blockedThread){
		blockedThread = GetThread(blockedExpiredThreads, tid);
		blockedThread->state = ABLE;
		AddThread(expiredThreads, blockedThread);
	}else{
		blockedThread->state = ABLE;
		AddThread(activeThreads, blockedThread);		
	}
	
	finishThread();

}
