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
int main(int argc, const char * argv[]) {
    // insert code here...
    if(argc != 1) {
        printf("Invalid argument count (%d). Exiting...", argc);
        exit(EXIT_FAILURE);
    }

    int pid = fork();
    if(pid == -1) {
        printf("Couldn't fork the parent process. Exiting...");
        exit(EXIT_FAILURE);
    }
    
    if(pid == 0) {
        close(STDOUT_FILENO);
        open("y", O_CREAT | O_WRONLY | O_TRUNC , DEFAULT_PERMISSION);
        char * args[2];
        args[0] = "/bin/ls";
        args[1] = NULL;
        execvp(args[0], args);
        printf("execp of program '%s' failed", args[0]);
    } else {
        wait(NULL);
    }
    
    return 0;
}
