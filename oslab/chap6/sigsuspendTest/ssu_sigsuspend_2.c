#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

static void ssu_func(int signo);
void ssu_print_mask(const char *str);

int main(void) {

	sigset_t new_mask, old_mask, wait_mask;
	ssu_print_mask("program start: ");

	if (signal(SIGINT, ssu_func) == SIG_ERR) {
		fprintf(stderr, "signal(SIGINT) error\n");
		exit(1);
	}

	sigemptyset(&wait_mask); //시그널 집합 비우기
	sigaddset(&wait_mask, SIGUSR1); //SIGUSR1 시그널 추가하기
	sigemptyset(&new_mask); //시그널 집합 비우기
	sigaddset(&new_mask, SIGINT); //SIGINT 시그널 추가하기

	if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
		fprintf(stderr, "SIG_BLOCK() error\n"); //sigmask 실행
		exit(1);
	}

	ssu_print_mask("in critical region: "); 

	if (sigsuspend(&wait_mask) != -1) { //대기상태로 만듦
		fprintf(stderr, "sigsuspend() error\n");
		exit(1);
	}

	ssu_print_mask("after return from sigsuspend: ");

	if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0 ) {
		fprintf(stderr, "SIG_SETMASK() error\n"); //이전 블록된 시그널 집합을 지우고 old_mask가 가리키는 시그널 집합의 값이 프로세스의 새 시그널 마스크로 설정
		exit(1);
	}

	ssu_print_mask("program exit: ");
	exit(0);
}

void ssu_print_mask(const char *str) {

	sigset_t sig_set;
	int err_num;

	err_num = errno;

	if (sigprocmask(0, NULL, &sig_set) < 0 ) {
		fprintf(stderr, "sigprocmask() error\n"); 
		exit(1);
	}

	printf("%s ", str);

	if (sigismember(&sig_set, SIGINT)) //SIGINT 존재여부
		printf("SIGINT ");
	
	if (sigismember(&sig_set, SIGQUIT)) //SIGQUIT 존재여부
		printf("SIGQUIT ");

	if (sigismember(&sig_set, SIGUSR1)) //SIGUSR1 존재여부
		printf("SIGUSR1 ");

	if (sigismember(&sig_set, SIGALRM)) //SIGALRM 존재여부
		printf("SIGARLM ");

	printf("\n");
	errno = err_num;
}

static void ssu_func(int signo) {

	ssu_print_mask("\nin ssu_func : ");
}