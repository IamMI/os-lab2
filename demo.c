#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>



int main(void)
{
	int* a = (int*)malloc(sizeof(int)*4);
	a[0] = 1;
	a[1] = 2;
	return 0;
}

