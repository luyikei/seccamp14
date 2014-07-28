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

#define TOTAL_CHILDREN 5

int pipe_fd[TOTAL_CHILDREN][2];


void do_child(int current, int next)
{
	char s[100];

	if(current == TOTAL_CHILDREN-1){
		close(pipe_fd[current][1]);
		dup2(pipe_fd[current][0], 0 );

		scanf("%s", s );
		printf("The last child says:%s\n", s);
		fflush(stdout);

	}else{
		close(pipe_fd[current][1]);
		dup2(pipe_fd[current][0], 0 );
		close(pipe_fd[next][0]);
		dup2(pipe_fd[next][1], 1);

		scanf("%s", s);
		printf("%s\n", s);
		fflush(stdout);

	}

	exit(0);
}

void do_parent(int current, int next)
{
	char s[100];
	int status,i;

	close(pipe_fd[next][0]);
	dup2(pipe_fd[next][1], 1);

	scanf("%s", s);
	printf("%s\n", s);
	fflush(stdout);

	/* write(pipe_fd[next][1], &s, sizeof(s)); */

	for(i=0;i<TOTAL_CHILDREN;i++){
		if(wait(&status) < 0){
			perror("wait");
			exit(1);
		}
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
		  do_parent(-1,0);
	}else if( children[i] == 0){
		  do_child(i, i+1);
	}



	return EXIT_SUCCESS;
}
