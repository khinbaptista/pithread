#include <ucontext.h>
#include "pidata.h"
#include "pithread.h"
#include "pithread_queue.h"

TCB_t* activeThreads = NULL;
TCB_t* expiredThreads = NULL;
TCB_t* blockedThreads = NULL;

//NEEDS THIS LIST TO VERIFY WHICH THREADS
//ARE BLOCKED BY MUTEX BECAUSE YOU DON'T
//HAVE ACCESS TO ALL MUTEXES
//(YOU COULD MAKE A MUTEX LIST INSTEAD TO
// HAVE A RECORD OF ALL MUTEXES CREATED
//ALONG THE EXECUTION OF A PROGRAM)
TCB_t* mutexBlockedThreads = NULL;

TCB_t* runningThread = NULL;

char* newStack = NULL;

ucontext_t finishCtx[1];
ucontext_t schedulerCtx[1];


WaitQueue_t* waitTids;


int counter;
int initialized;
int mutexBlock = 0;


// Helper functions signatures

void Initialize(void);
void DecreaseCredits(TCB_t* t);
void schedule();
void finishThread();
void unblock();


// Implementations

void Initialize(){
	if (initialized == 1) return;

	char mainStack[SIGSTKSZ];
	char finishStack[SIGSTKSZ];
	char schedulerStack[SIGSTKSZ];

	initialized = 1;
	counter = 0;

	//FINISH THREAD CONTEXT CREATION
	getcontext(finishCtx);
	finishCtx->uc_link = NULL;
	finishCtx->uc_stack.ss_sp = finishStack;
	finishCtx->uc_stack.ss_size = SIGSTKSZ;
	finishCtx->uc_stack.ss_flags = 0;

	makecontext(finishCtx, (void (*)(void)) finishThread, 0);

	//SCHEDULER CONTEXT CREATION
	getcontext(schedulerCtx);
	schedulerCtx->uc_link = NULL;
	schedulerCtx->uc_stack.ss_sp = schedulerStack;
	schedulerCtx->uc_stack.ss_size = SIGSTKSZ;
	schedulerCtx->uc_stack.ss_flags = 0;

	makecontext(schedulerCtx, (void (*)(void)) schedule, 0);

	//MAIN THREAD CREATION
	TCB_t* mainThread = (TCB_t*)malloc(sizeof(TCB_t));

	mainThread->tid = counter++;
	mainThread->state = EXECUTION;
	mainThread->credCreate = 100;
	mainThread->credReal = 100;

	getcontext(&mainThread->context);

	mainThread->context.uc_link = finishCtx;
	mainThread->context.uc_stack.ss_sp = mainStack;
	mainThread->context.uc_stack.ss_size = SIGSTKSZ;
	mainThread->context.uc_stack.ss_flags = 0;

	runningThread = mainThread;
}

int picreate(int cred, void* (*entry)(void*), void *arg){
	Initialize();
	newStack =(char*)malloc(SIGSTKSZ*(sizeof(char)));
	TCB_t* thread;

	if( ( thread = (TCB_t*)malloc(sizeof(TCB_t)) ) ){
		// NEW THREAD CREATION
		thread->tid = counter++;
		thread->state = ABLE;	// precisa ter um state "CREATION"?
		thread->credCreate = cred;
		thread->credReal = cred;

		getcontext(&thread->context);
		thread->context.uc_link = finishCtx;

		thread->context.uc_stack.ss_sp = newStack;
		thread->context.uc_stack.ss_size = SIGSTKSZ;
		thread->context.uc_stack.ss_flags = 0;

		makecontext(&thread->context, (void (*)(void)) entry, 1, arg);

		activeThreads = AddThread(activeThreads, thread);

		
		return thread->tid;
	}

	
	return -1;
}

int piwait(int tid){
	Initialize();
	
	TCB_t* waitedThread;

	//TRY TO GET THREAD TO BE WAITED FOR FROM
	//THREAD LISTS
	waitedThread = GetThread(activeThreads, tid);
	
	if(!waitedThread){
		if(!waitedThread){
			waitedThread = GetThread(blockedThreads, tid);
			if(!waitedThread){
				waitedThread = GetThread(mutexBlockedThreads, tid);
			}
		}
	}

	//IF FOUND THE SPECIFIED TID IN THREAD LISTS
	if(waitedThread){
		
		//THE SPECIFIED TID CAN'T BE WAITED FOR
		//ANOTHER THREAD
		if(!GetWait(waitTids, tid)){
			waitTids = AddWait(waitTids, tid, runningThread->tid);

			runningThread->state = BLOCKED;
		
			swapcontext(&runningThread->context, schedulerCtx);
			return 0;
		}
	}

	return -1;
}

int piyield(){
	Initialize();
	runningThread->state = ABLE;
	
	swapcontext(&runningThread->context, schedulerCtx);

	
	return 0;
}

int pimutex_init(pimutex_t *mtx){
	Initialize();
	
	if (mtx != NULL){
		mtx->flag = 1;
		mtx->first = NULL;
		mtx->last = NULL;
		
		return 0;
	}

	return -1;
}

int pilock(pimutex_t *mtx){
	Initialize();

	
	if(mtx != NULL){
		//IF MUTEX IS ALREADY LOCKED BY ANOTHER THREAD
		if(mtx->flag == 0){
			
			
			runningThread->state = BLOCKED;
			mutexBlock = 1;
			//PUTS THREAD IN MUTEX LOCKED LIST
			mutexBlockedThreads = AddThread(mutexBlockedThreads, runningThread);
			mtx->first = AddToMutex(mtx->first, runningThread);

			swapcontext(&runningThread->context, schedulerCtx);
		}
		
		//LOCK MUTEX
		mtx->flag = 0;

		return 0;
	}
	
	return -1;
}

int piunlock (pimutex_t *mtx){
	Initialize();	
	
	TCB_t* blocked;
	
	
	if(mtx != NULL){
		//IF MUTEX IS ALREADY LOCKED BY ANOTHER THREAD
		if(mtx->flag == 0){
			
			//UNLOCK MUTEX
			mtx->flag = 1;
			blocked = mtx->first;
			
			if(blocked){
				
				blocked->next = NULL;
				mutexBlockedThreads = RemoveThread(mutexBlockedThreads, blocked->tid);
				
				mtx->first = RemoveFromMutex(mtx->first);
				
				if(blocked->credReal){
					activeThreads = AddThread(activeThreads, blocked);
				}
				else{
					expiredThreads = AddThread(expiredThreads, blocked);
				}
			}
			return 0;
		}
	}
	
	
	return -1;
}

// Helper functions implementations


void DecreaseCredits(TCB_t* t){
	if (!t) return;
	
	
	int credits = t->credReal;
	credits -= EXECUTION_COST;

	if (credits < 0) credits = 0;

	t->credReal = credits;
}

// WHEN THREAD FINISHES
void finishThread(){

	unblock();


	runningThread->state = FINISHED;

	schedule();
}

// SELECTS NEXT RUNNING THREAD
void schedule(){
	
	// IF RUNNING THREAD IS NOT FINISHED
	// PUTS THE THREAD IN ONE OF THE TWO
	// ABLE QUEUES
	if(runningThread->state != FINISHED){
		DecreaseCredits(runningThread);
		
		if(runningThread->state == ABLE){

			if (runningThread->credReal == 0){
				expiredThreads = AddThread(expiredThreads, runningThread);
			}
			else{
				activeThreads = AddThread(activeThreads, runningThread);
			}
		}
		else{
			if(runningThread->state == BLOCKED){
				if(mutexBlock){
					//if it was blocked while locking a mutex
					//reset the flag and it was added to another queue
					//in the pilock function
					mutexBlock = 0;
				}
				else{
					blockedThreads = AddThread(blockedThreads, runningThread);
				}
			}
		}
	}
	else{
		// IF THE THREAD IS FINISHED,
		// FREE THE TCB MEMORY
		
		free(runningThread);
	}

	// SELECTS NEXT THREAD TO RUN

	runningThread = activeThreads;
	if (runningThread == NULL){
		
		SwapQueues(&activeThreads, &expiredThreads);

		activeThreads = RestoreCredits(activeThreads);

		runningThread = activeThreads;
	}
	
	activeThreads = activeThreads->next;
	runningThread->next = NULL;
	runningThread->prev = NULL;

	if(activeThreads)
		activeThreads->prev = NULL;

	runningThread->state = EXECUTION;

	
	setcontext(&runningThread->context);
}

void unblock(){
	if (!CheckInit()) return;
	
	
	TCB_t* blockedThread = NULL;
	WaitQueue_t* waited = NULL;

	//IF THERE ARE THREADS TO BE WAITED FOR
	if(waitTids){

		//VERIFY IF THE FINISHED THREAD IS WAITED
		//FOR ANOTHER THREAD
		waited = GetWait(waitTids,runningThread->tid);
		if(waited){

			//FIND THE CORRESPONDING BLOCKED THREAD,
			//REMOVES IT FROM THE BLOCKED THREADS LIST
			//AND ADDS IT TO THE ABLE THREADS LIST
			blockedThread = GetThread(blockedThreads, waited->waiting);

			blockedThread->state = ABLE;

			blockedThreads = RemoveThread(blockedThreads, blockedThread->tid);
			waitTids = RemoveWait(waitTids,runningThread->tid);

			activeThreads = AddThread(activeThreads, blockedThread);
		}
	}
}
