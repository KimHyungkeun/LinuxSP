#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_thread1(void *arg);
void *ssu_thread2(void *arg);

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

int count = 0;
int input = 0;
int t1 = 0, t2 = 0;

int main(void) {

	pthread_t tid1, tid2;
	int status;

	if (pthread_create(&tid1, NULL, ssu_thread1, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n"); //첫번째 스레드 생성
		exit(1);
	}

	if (pthread_create(&tid2, NULL, ssu_thread2, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n"); //두번째 스레드 생성
		exit(1);
	}

	while (1) {
		printf("2개 이상의 개수 입력 : ");
		scanf("%d", &input); //숫자 입력

		if(input >= 2) {
			pthread_cond_signal(&cond1); 
			break;
		}
	}

	pthread_join(tid1, (void *)&status); //쓰레드1 조인
	pthread_join(tid2, (void *)&status); //쓰레드2 조인

	printf("complete \n");
	exit(0);
}

void *ssu_thread1(void *arg) {

	while(1) {
		pthread_mutex_lock(&mutex1); //뮤텍스 잠금

		if (input < 2)
			pthread_cond_wait(&cond1, &mutex1); //쓰레드를 대기시킨다

		if (input == count) {
			pthread_cond_signal(&cond2);
			break;
		}

		if (count == 0) {
			t2++; 
			count++;
			printf("Thread 1 : %d\n", t1);
		}

		else if (count % 2 == 0) {
			t1 += t2;
			count++;
			printf("Thread 1 :%d\n", t1);
		}

		pthread_cond_signal(&cond2); //쓰레드2에 대한 컨디션
		pthread_cond_wait(&cond1, &mutex1);
		pthread_mutex_unlock(&mutex1); //뮤텍스 잠금해제
	}

	return NULL;
}

void *ssu_thread2(void *arg) {

	while(1) {
		pthread_mutex_lock(&mutex2); //뮤텍스 잠금

		if (input < 2) 
			pthread_cond_wait(&cond2, &mutex2); 

		if (input == count) {
			pthread_cond_signal(&cond1);
			break;
		}

		if (count == 1) {
			count++;
			printf("Thread 2 : %d\n", t2);
		}
		else if (count % 2 == 1) {
			t2 += t1;
			count++;
			printf("Thread 2 : %d\n", t2);
		}

		pthread_cond_signal(&cond1); //쓰레드2에 대한 컨디션
		pthread_cond_wait(&cond2, &mutex2);
		pthread_mutex_unlock(&mutex2); //뮤텍스 잠금해제

	}

	return NULL;
}
