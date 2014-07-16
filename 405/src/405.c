/*
 ============================================================================
 Name        : 405.c
 Author      : luyikei
 Version     : pre-alpha
 Copyright   : NO=LICENSE
 Description : http://www.coins.tsukuba.ac.jp/~syspro/2013/shui/quizzes4.html 405
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define STDOUT 1
#define STDIN 0

int
main(int argc, char *argv[])
{
	int file_fd, out_file_fd;

	if (argc != 3) {
		  printf("Usage: %s file_name filename\n", argv[0]);
		  exit(1);
	}

	file_fd = open(argv[1], O_RDONLY);
	if (file_fd < 0) {
		  perror("open");
		  close(file_fd);
		  exit(1);
	}

	out_file_fd = open(argv[2], O_CREAT | O_WRONLY, 0666);
	if (out_file_fd < 0) {
		  perror("open");
		  close(out_file_fd);
		  exit(1);
	}

	close(0);
	close(1);



	if (dup2(file_fd, 0) < 0) {
		  perror("dup2");
		  close(file_fd);
		  exit(1);
	}
	close(file_fd);
	if (dup2(out_file_fd, 1) < 0) {
			  perror("dup2");
			  close(out_file_fd);
			  exit(1);
	}
	close(out_file_fd);

	execlp("wc", "wc", NULL);
	return 0;
}
