#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

void ssu_signal_handler(int signo);

jmp_buf jump_buffer;

int main(void) {

	signal(SIGINT, ssu_signal_handler);

	while(1) {

		if(setjmp(jump_buffer) == 0) { //setjmp 실행
			printf("Hit Ctrl-c at anytime ... \n");
			pause();
		}
	}

	exit(0);
}

void ssu_signal_handler(int signo) {
	char character;

	signal(signo, SIG_IGN); //시그널 무시
	printf("Did you hit Ctrl-c?\n" "Do you really want to quit? [y/n] ");
	character = getchar(); //문자 입력

	if(character == 'y' || character == 'Y')
		exit(0);
	else {
		signal(SIGINT, ssu_signal_handler); 
		longjmp(jump_buffer, 1); //longjmp 실행
	}
}
