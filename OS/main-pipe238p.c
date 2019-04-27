//
//  cp238.c
//  OSAss1
//
//  Created by Yogesh Jadhav on 07/10/18.
//  Copyright © 2018 Yogesh Jadhav. All rights reserved.
//

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int DEFAULT_PERMISSION = 0666;
int LIMIT = 1024;
int main(int argc, const char * argv[]) {
    // insert code here...
    if(argc != 1) {
        printf("Invalid argument count (%d). Exiting...", argc);
        exit(EXIT_FAILURE);
    }
    
    int pipes[2];
    pipe(pipes);
    int pipes2[2];
    pipe(pipes2);
    
    int pid = fork();
    if(pid == -1) {
        printf("Couldn't fork the parent process. Exiting...");
        exit(EXIT_FAILURE);
    }
    
    if(pid == 0) {
        close(1);
        dup(pipes[1]);
        close(pipes[0]);
        close(pipes[1]);
        close(0);
        char * args[2];
        args[0] = "ls";
        args[1] = NULL;
        execvp(args[0], args);
        printf("execp of program '%s' failed", args[0]);
        exit(EXIT_FAILURE);
    } else {
    
        wait(NULL);
        int pid2 = fork();
        if(pid2 == -1) {
            printf("Couldn't fork the child process. Exiting...");
            exit(EXIT_FAILURE);
        }

        if(pid2 == 0) {
            
            close(0);
            dup(pipes[0]);
            close(1);
            dup(pipes2[1]);
            
            close(pipes[0]);
            close(pipes[1]);
            close(pipes2[0]);
            close(pipes2[1]);
            
            char * args[3];
            args[0] = "grep";
            args[1] = "main";
            args[2] = NULL;
            execvp(args[0], args);
            printf("execp of program '%s' failed", args[0]);
            exit(EXIT_FAILURE);
        } else {

            close(0);
            dup(pipes2[0]);
            
            close(pipes[0]);
            close(pipes[1]);
            close(pipes2[0]);
            close(pipes2[1]);
            
            wait(NULL);

            char * args[2];
            args[0] = "wc";
            args[1] = NULL;
            
            execvp(args[0], args);
            printf("execp of program '%s' failed", args[0]);
            exit(EXIT_FAILURE);
        }
        
    }
    
    return 0;
}
