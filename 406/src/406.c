/*
 ============================================================================
 Name        : 406.c
 Author      : luyikei
 Version     : pre-alpha
 Copyright   : NO=LICENSE
 Description : http://www.coins.tsukuba.ac.jp/~syspro/2013/shui/quizzes4.html 406
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

extern char **environ;

void
do_child(char *command)
{
	char *argv[4];

	argv[0] = "/bin/bash";
	argv[1] = "-c";
	argv[2] = command;

	argv[3] = NULL;

	if (execve(argv[0], argv, environ) < 0){
		perror("execve");
		exit(1);
	}


}

int
mysystem(char *command)
{
	int child,status;

	if((child = fork()) < 0){
		perror("fork");
		exit(1);
	}

	if(child == 0)
		do_child(command);
	else {
		if(wait(&status) < 0){
			perror("wait");
			exit(1);
		}
	}

	return WEXITSTATUS(status);

}

int
main(int argc, char *argv[])
{
	if(argc != 4){
		puts("Four arguments is needed.");
		exit(1);
	}
	if(!mysystem(argv[1])){
		mysystem(argv[2]);
	}else{
		mysystem(argv[3]);
	}

	return 0;
}
