#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
	
	pid_t pgid;
	pid_t pid;

	pid = getpid(); //process id 
	pgid = getpgrp(); //process group id
	printf("pid : %d, pgid : %d\n", pid, pgid);
	exit(0);
}


