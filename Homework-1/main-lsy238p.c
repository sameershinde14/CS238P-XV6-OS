#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int dest;
	if(argc != 1) {
		printf("Invalid number of arguments. Expected 2, Receieived:%d", argc);
		exit(EXIT_FAILURE);
	}
	close(STDOUT_FILENO);
	dest = open("y",  O_WRONLY | O_CREAT | O_TRUNC, 0666); 
  	char *command[] = { "/bin/ls",  NULL };
	execvp(command[0], command);
	return 0;
}


