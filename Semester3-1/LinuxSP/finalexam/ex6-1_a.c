#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void signal_handler(int signo) {
	printf("SIGINT handler!!\n");
	exit(1);
}

int main(int argc, char* argv[]) {

	int i;

	signal(SIGINT, signal_handler);

	while(1) { 
	for (i=0;i<argc;i++) 
		printf("ARGV[%d] : %s\n", i, argv[i]);
		
		sleep(1);
	
	}
	
	

	exit(0);

}
