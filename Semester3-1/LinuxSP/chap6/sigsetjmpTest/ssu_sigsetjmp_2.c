#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>

static void ssu_alarm(int signo);
static void ssu_func(int singo);
void ssu_mask(const char *str);

static volatile sig_atomic_t can_jump;
static sigjmp_buf jump_buf;

int main(void) {

	if (signal(SIGUSR1, ssu_func) == SIG_ERR) {
		fprintf(stderr, "SIGUSR1 error"); //SIGUSR1 시그널 보냄
		exit(1);
	}

	if (signal(SIGALRM, ssu_alarm) == SIG_ERR) {
		fprintf(stderr, "SIGALRM error"); //SIGALRM 시그널 보냄
		exit(1);
	}

	ssu_mask("starting main: ");

	if (sigsetjmp(jump_buf, 1)) { //setjmp 실행
		ssu_mask("ending main: "); 
		exit(0);
	}

	can_jump = 1;

	while(1)
		pause();

	exit(0);
}


void ssu_mask(const char *str) {

	sigset_t sig_set;
	int err_num;

	err_num = errno; //errno 설정

	if (sigprocmask(0, NULL, &sig_set) < 0) {
		printf("sigprocmask() error");
		exit(1);
	}

	printf("%s", str);

	if (sigismember(&sig_set, SIGINT))
		printf("SIGINT "); //SIGINT 멤버여부 확인

	if (sigismember(&sig_set, SIGQUIT))
		printf("SIGQUIT "); //SIGQUIT 멤버여부 확인

	if (sigismember(&sig_set, SIGUSR1))
		printf("SIGUSR1 "); //SIGUSR1 멤버여부 확인

	if (sigismember(&sig_set, SIGALRM))
		printf("SIGALRM "); //SIGALRM 멤버여부 확인

	printf("\n");
	errno = err_num;

}

static void ssu_func(int signo) {

	time_t start_time;

	if (can_jump == 0)
		return;

	ssu_mask("starting ssu_func : ");
	alarm(3); //3초뒤 알람
	start_time = time(NULL);

	while(1)
		if (time(NULL) > start_time + 5)
			break;

	ssu_mask("ending ssu_func: ");
	can_jump = 0;
	siglongjmp(jump_buf, 1); //longjmp 실행

}

static void ssu_alarm(int signo) {
	ssu_mask("in ssu_alarm: ");
}
