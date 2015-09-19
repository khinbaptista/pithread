/*
	Queue manipulation functions
*/

#pragma once

#include "pidata.h"


typedef struct WaitQueue {
	int	tid;
	int	waiting;
	struct WaitQueue 	*next;
} WaitQueue_t;

TCB_t* NextThread(TCB_t* queue);
TCB_t* AddThread(TCB_t* queue, TCB_t* thread);
TCB_t* AddToMutex(TCB_t* queue, TCB_t* thread);
void SwapQueues(TCB_t** a, TCB_t** b);
TCB_t* GetThread(TCB_t* queue, int tid);
TCB_t* RemoveThread(TCB_t* queue, int tid);
TCB_t* RestoreCredits(TCB_t* queue);
WaitQueue_t* AddWait(WaitQueue_t* queue, int tid, int waiting);
WaitQueue_t* GetWait(WaitQueue_t* queue, int tid);
WaitQueue_t* RemoveWait(WaitQueue_t* queue, int tid);
