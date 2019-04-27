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
    if(argc != 1){
        printf("Number of arguments not matching. Expected: 1, Received:%d", argc);
        exit(EXIT_FAILURE);
    }
    int pipes[2];
    pipe(pipes);
    int pipesChild[2];
    pipe(pipesChild);

    int pid = fork();
    if(pid == -1){
        printf("Fork failed. Exiting..");
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        close(1);
        dup(pipes[1]);
        close(pipes[0]);
        close(pipes[1]);
        close(0);
        char * command[2] = {"ls", NULL};
        execvp(command[0], command);
        printf("execvp for %s failed. Exiting", command[0]);
        exit(EXIT_FAILURE);
    }
    else{
        wait(NULL);
        int pid2 = fork();
        if(pid2 == -1){
            printf("Fork failed. Exiting..");
            exit(EXIT_FAILURE);
        }
        if(pid2 == 0){
            close(0);
            dup(pipes[0]);
            close(1);
            dup(pipesChild[1]);
            close(pipes[0]);
            close(pipes[1]);
            close(pipesChild[0]);
            close(pipesChild[1]);

            char * command[3] = {"grep", "main", NULL};
            execvp(command[0], command);
            printf("execvp for %s failed. Exiting", command[0]);
            exit(EXIT_FAILURE);
        }
        else{
            close(0);
            dup(pipesChild[0]);
            close(pipes[0]);
            close(pipes[1]);
            close(pipesChild[0]);
            close(pipesChild[1]);

            wait(NULL);
            char *command[2] = {"wc", NULL};
            execvp(command[0], command);
            printf("execvp for %s failed. Exiting", command[0]);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}


