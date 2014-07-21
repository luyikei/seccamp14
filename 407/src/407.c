/*
 ============================================================================
 Name        : 407.c
 Author      : luyikei
 Version     :
 Copyright   : NO-LICENSE
 Description : http://www.coins.tsukuba.ac.jp/~syspro/2013/shui/quizzes4.html 407
 ============================================================================
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>

#include <limits.h>

int pipe_fd[2];
int total;
fd_set fds;
int pipe_size;

void print_num_rec(int *num)
{
	putchar('0' + *num % ('9' - '0' + 1));
	fflush(stdout);
	(*num)++;
}

void print_alpha_rec(int *num)
{
	putchar('A' + *num % ('Z' - 'A' + 1));
	fflush(stdout);
	(*num)++;
}

void do_child()
{
	//char c='\0';
	int t=0;
	int i=total;
	int r;
	char *data;

	data = (char*)malloc(pipe_size);
	if (data == NULL) exit(1);

	memset(data, 0 ,pipe_size);

	close(pipe_fd[0]);

	while (i > 0) {

		/* パイプにデータが無くなってから write */
		if (write(pipe_fd[1], &i, sizeof(i)) < 0) {
			  perror("write");
			  exit(1);
		}
		print_num_rec(&t);

		/* パイプを満タンにして、次の write を 親プロセスがアルファベットを出力し、データを read して開放されるまで block する。 */
		if (write(pipe_fd[1], data, pipe_size) < 0) {
			  perror("write");
			  exit(1);
		}
		i-=2;
	}
}

void do_parent()
{
	char *data;
	int    i=0;
	int     count, status, t=0;

	data = (char*)malloc(pipe_size);
	if (data == NULL) exit(1);

	close(pipe_fd[1]);

	/* まず i を read */
	while ((count = read(pipe_fd[0], &i, sizeof(i))) > 0) {
		/* パイプにデータがあるとき(満タンの時)、かつ i > 1のとき 出力 する。  */
		if(select(pipe_fd[0]+1, &fds, NULL, NULL, NULL) > 0 && i > 1){
			print_alpha_rec(&t);
		}

		/* 満タンパイプを開放 */
		read(pipe_fd[0], data, pipe_size);
	}

	if (count < 0) {
		  perror("read");
		  exit(1);
	}

	if (wait(&status) < 0) {
		  perror("wait");
		  exit(1);
	}
}

void output()
{
	int child;
	if (pipe(pipe_fd) < 0) {
		  perror("pipe");
		  exit(1);
	}

	FD_ZERO(&fds);
	FD_SET(pipe_fd[0], &fds);

	//fcntl(pipe_fd[1], F_SETFL, O_NONBLOCK);

	//fcntl(pipe_fd[0], F_SETFL, O_NONBLOCK);

	pipe_size = fcntl(pipe_fd[1], F_GETPIPE_SZ);

	if ((child = fork()) < 0) {
		  perror("fork");
		  exit(1);
	}

	if (child)
		  do_parent();
	else
		  do_child();
}

int main(int argc, char *argv[])
{
	if(argc != 2){
		exit(1);
	}
	total = atoi(argv[1]);

	output();


	return 0;
}
