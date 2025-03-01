#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define SECOND_TO_MICRO 1000000

void ssu_runtime (struct timeval* begin_t, struct timeval* end_t) {

        end_t -> tv_sec -= begin_t -> tv_sec;

        if(end_t -> tv_usec < begin_t -> tv_usec) {
                end_t -> tv_sec--;
                end_t -> tv_usec += SECOND_TO_MICRO;
        }

        end_t -> tv_usec -= begin_t -> tv_usec;
        printf("B_Runtime : %ld:%06ld(sec:usec)\n",end_t -> tv_sec, end_t -> tv_usec);

}


int main(void) {

char character;
int fd;
int line_count = 0;
struct timeval begin_t, end_t;

gettimeofday(&begin_t, NULL);

if((fd = open("ssu_test.txt",O_RDONLY)) < 0){

	fprintf(stderr,"open error for %s\n","ssu_test.txt");
	exit(1);
}

while(1) {

	if(read(fd,&character,1) > 0){

		if(character == '\n')
			line_count++;
	}

	else
		break;
}

printf("Total line : %d\n",line_count);

gettimeofday(&end_t,NULL);
ssu_runtime(&begin_t, &end_t);
exit(0);

}
