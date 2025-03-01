#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_pthread(void *arg);

int main(void) {

	pthread_t tid;

	if (pthread_create(&tid, NULL, ssu_pthread, NULL) != 0) {
		fprintf(stderr, "create error\n");
		exit(1);
	}

	printf("%u\n", (unsigned int) tid);

	if (pthread_create(&tid, NULL, ssu_pthread, NULL) != 0) {
		fprintf(stderr, "create error\n");
		exit(1);
	}

	printf("%u\n", (unsigned int) tid);

	sleep(1);
	exit(0);
}

void *ssu_pthread(void *arg) {

	pthread_t tid;

	tid = pthread_self();
	printf("->%u\n", (unsigned int)tid);

	return NULL;
}
