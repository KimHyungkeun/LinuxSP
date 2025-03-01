#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024
#define SECOND_TO_MICRO 1000000

void ssu_runtime (struct timeval* begin_t, struct timeval* end_t) {

        end_t -> tv_sec -= begin_t -> tv_sec;

        if(end_t -> tv_usec < begin_t -> tv_usec) {
                end_t -> tv_sec--;
                end_t -> tv_usec += SECOND_TO_MICRO;
        }

        end_t -> tv_usec -= begin_t -> tv_usec;
        printf("Runtime : %ld:%06ld(sec:usec)\n",end_t -> tv_sec, end_t -> tv_usec);

}



int main(void){
	struct timeval begin_t, end_t;
	char *fname = "ssu_creat.txt";
	int fd;

	gettimeofday(&begin_t,NULL);

	if((fd = creat(fname, 0666)) < 0) {
		fprintf(stderr,"creat error for %s\n",fname);
		gettimeofday(&end_t,NULL);
		ssu_runtime(&begin_t,&end_t);
		exit(1);
	}

		

	printf("Fisrt printf is on the screen.\n");
	gettimeofday(&end_t,NULL);
        ssu_runtime(&begin_t,&end_t);

	dup2(fd,1);
	printf("Second printf is in this file.\n");
	exit(0);

}


