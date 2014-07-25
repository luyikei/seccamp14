/*
 ============================================================================
 Name        : 408.c
 Author      : luyikei
 Version     :
 Copyright   : NO-LICENSE
 Description : http://www.coins.tsukuba.ac.jp/~syspro/2013/shui/quizzes4.html 408
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

int **pipe_fd;
int total;
int c_count;
int *children;

void print_rec(int num, int alpha, int is_num)
{
	if(is_num){
		putchar('0' + num % ('9' - '0' + 1));
		fflush(stdout);
	}else{
		putchar('A' + num % ('Z' - 'A' + 1));
		fflush(stdout);
	}
}

void do_child(int current, int next)
{
	int i[3],count;

	close(pipe_fd[current][1]);
	close(pipe_fd[next][0]);

	while(1){

		count = read(pipe_fd[current][0], &i, sizeof(i));

		if (count < 0) {
			  perror("read");
			  exit(1);
		}

		if(i[0] + i[1] >= total){
			if(i[2]){
				i[2]=0;
				i[1]++;
			}else{
				i[2]=1;
				i[0]++;
			}

			if (write(pipe_fd[next][1], &i, sizeof(i)) < 0) {
				  perror("write");
				  exit(1);
			}

			break;
		}

		print_rec(i[0], i[1], i[2]);

		if(i[2]){
			i[2]=0;
			i[1]++;
		}else{
			i[2]=1;
			i[0]++;
		}

		if (write(pipe_fd[next][1], &i, sizeof(i)) < 0) {
			  perror("write");
			  exit(1);
		}

	}
	exit(0);
}

void do_parent(int current, int next)
{
	int    i[3]={0,0,1};
	int    status, count;

	close(pipe_fd[next][0]);

	close(pipe_fd[current][1]);


	while(1){

		if(i[0] + i[1] >= total){
			if(i[2]){
				i[2]=0;
				i[1]++;
			}else{
				i[2]=1;
				i[0]++;
			}

			if (write(pipe_fd[next][1], &i, sizeof(i)) < 0) {
				  perror("write");
				  exit(1);
			}

			break;
		}

		print_rec(i[0], i[1], i[2]);

		if(i[2]){
			i[2]=0;
			i[1]++;
		}else{
			i[2]=1;
			i[0]++;
		}

		if (write(pipe_fd[next][1], &i, sizeof(i)) < 0) {
			  perror("write");
			  exit(1);
		}



		count = read(pipe_fd[current][0], &i, sizeof(i));

		if (count < 0) {
			  perror("read");
			  exit(1);
		}

		next++;
		if(next == c_count)
			next=0;

	}

	if (wait(&status) < 0) {
		  perror("wait");
		  exit(1);
	}
}

void output()
{
	int i;
	children = malloc(sizeof(int) * c_count);

	pipe_fd = malloc(sizeof(int *) * (c_count+1));

	for(i =0;i<c_count+1;i++){
		pipe_fd[i] = malloc(sizeof(int) * 2);
	}

	for(i =0;i<c_count+1;i++){
		if (pipe(pipe_fd[i]) < 0) {
			  perror("pipe");
			  exit(1);
		}
	}

	for( i=0 ; i< c_count && (children[i] = fork()) > 0 ; i++ );

	if( i == c_count ){
		  do_parent(i, 0);
	}else if( children[i] == 0){
		  do_child(i, c_count);
	}

	for (i=0;i<(c_count+1);i++) {
		free(pipe_fd[i]);
	}
	free(pipe_fd);
	free(children);
}

int main(int argc, char *argv[])
{
	if(argc != 3){
		exit(1);
	}
	total = atoi(argv[1]);
	c_count = atoi(argv[2]);

	output();


	return 0;
}
