#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

	if(argc != 3) {
		fprintf(stderr, "usage %s <oldname><newfile>\n", argv[0]);
		exit(1);
	}

	if(link(argv[1], argv[2]) < 0) {
		fprintf(stderr, "link error\n");
		exit(0);
	}

	if(unlink(argv[1]) < 0) {
		fprintf(stderr, "unlink error\n");
		exit(0);
	}

	exit(0);
}
