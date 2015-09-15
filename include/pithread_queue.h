/*
	Queue manipulation functions
*/

#pragma once

#include "pidata.h"

TCB_t* NextThread(TCB_t** queue);
void AddThread(TCB_t** queue, TCB_t* thread);
void SwapQueues(TCB_t** a, TCB_t** b);
TCB_t* GetThread(TCB_t** queue, int tid);
