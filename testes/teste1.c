//#include <stdlib.h>
#include <stdio.h>
#include "pithread.h"

void* func1(void *arg){
	int i = 0;
	printf("Index initialized with 0\n");
	
	for (; i < 10; i++){
		printf("thread 1 with index %d\n\n", i);

		if (i % 5 == 0) piyield();
	}
	
	return 0;
}

void* func2(void* arg){
	int i = 0;
	
	for (; i < 10; i++){
		printf("thread 2\n\n");
		
		//if (i % 5 == 0) piyield();
	}
	
	return 0;
}

int main(int argc, char* argv[]){
	
	printf("Create new thread.\n");
	int t1, t2;
	
	t1 = picreate(1, func1, 0);
	t2 = picreate(1, func2, 0);
	
	printf("Threads created. Waiting...\n");
	piwait(t1);
	piwait(t2);
	
	printf("Threads finished. Exiting...\n");
	return 0;
}
