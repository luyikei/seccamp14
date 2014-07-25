/*
 ============================================================================
 Name        : 409.c
 Author      : luyikei
 Version     :
 Copyright   : NO-LICENSE
 Description : http://www.coins.tsukuba.ac.jp/~syspro/2013/shui/quizzes4.html 409
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

#define TOTAL_CHILDREN 2

int pipe_fd[TOTAL_CHILDREN][2];

void do_child(int current, int next)
{
	if(current){

		close(0);
		close(1);
		dup2(pipe_fd[current][0], pipe_fd[current][0]);
		dup2(pipe_fd[current][1], pipe_fd[next][1]);

	}else{
		close(pipe_fd[next][0]);
		dup2(1, pipe_fd[next][1]);

	}
}

void do_parent()
{
	int status;

	close(pipe_fd[TOTAL_CHILDREN-1][1]);

	close(0);
	dup2(pipe_fd[TOTAL_CHILDREN-1][0], 0);

	if(wait(&status) < 0){
		perror("wait");
		exit(1);
	}
}

int main(void) {
	int children[TOTAL_CHILDREN];
	int i;

	for(i=0;i<TOTAL_CHILDREN;i++){
		if(pipe(pipe_fd[i]) < 0) {
			  perror("pipe");
			  exit(1);
		}
	}


	for( i=0 ; i< TOTAL_CHILDREN && (children[i] = fork()) > 0 ; i++ );

	if( i == TOTAL_CHILDREN ){
		  do_parent();
	}else if( children[i] == 0){
		  do_child(i-1, i);
	}



	return EXIT_SUCCESS;
}
