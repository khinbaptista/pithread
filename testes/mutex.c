#include <stdio.h>
#include "pithread.h"

// shared variables
int a;

// pithread bariables
int id1, id2;
pimutex_t mutex;

// thread functions

void* func1(void* arg){
	printf("\nThread 1 started.\n");
	pilock(&mutex);
		printf("Thread 1 inside restricted area\n");
		a -= 10;
		printf("a = %d\n", a);
		printf("Thread 1 leaving restricted area\n");
	piunlock(&mutex);
	
	printf("Thread 1 exiting...\n");
	return 0;
}

void* func2(void* arg){
	printf("\nThread 2 started.\n");
	pilock(&mutex);
		printf("Thread 2 entered restricted area\n");
		piyield();
		
		a += 30;
		printf("a = %d\n", a);
		printf("Thread 2 leaving restricted area\n");
	piunlock(&mutex);
	
	printf("Thread 2 exiting...\n");
	return 0;
}


// main

int main(int argc, char* argv[]){
	a = 0;
	
	printf("\nInitializing mutex... ");
	pimutex_init(&mutex);
	
	if (&mutex == NULL){ printf("Pointers hate you.\n"); }
	
	printf("\nCreating threads... ");
	id1 = picreate(20, func1, 0);
	id2 = picreate(30, func2, 0);
	printf("Created threads %d and %d\n", id1, id2);
	
	printf("Waiting threads to finish...\n");
	piwait(id1);
	
	piwait(id2);
	
	printf("\na = %d\n", a);
	printf("Returned to main. Exiting...\n");
	
	return 0;
}
