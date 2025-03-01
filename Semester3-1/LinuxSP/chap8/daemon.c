#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

int daemon_test(void);
int main(void) {

	pid_t pid;

	pid = getpid();
	printf("parent process : %d\n", pid);
	printf("daemon process initialization\n");

	if (daemon_test() < 0){
		fprintf(stderr, "daemon init failed\n");
		exit(1);
	}

	exit(0);
}

int daemon_test(void) {

	pid_t pid;
	int fd, maxfd;


	if((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid != 0)
		exit(0);

	pid = getpid();
	printf("process %d running as daemon\n", pid);

	setsid();
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);

	maxfd = getdtablesize();

	for (fd = 0 ; fd < maxfd ; fd++)
		close(fd);

	umask(0);
	chdir("/");

	fd = open("/dev/null", O_RDWR);
	dup(0);
	dup(0);

	return 0;
}
