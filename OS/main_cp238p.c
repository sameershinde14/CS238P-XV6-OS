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

void doCp(int[2]);
int DEFAULT_PERMISSION = 0666;

int main(int argc, const char * argv[]) {
    // insert code here...
    if(argc != 3) {
        printf("Invalid argument count (%d). Exiting...", argc);
        exit(EXIT_FAILURE);
    }
    
    int fd[2];
    fd[0] = open(argv[1], O_RDONLY);
    fd[1] = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC , DEFAULT_PERMISSION);
    
    doCp(fd);
    
    close(fd[0]);
    close(fd[1]);
    
    return 0;
}

void doCp(int fd[2]) {
    ssize_t bytes_read = 1;
    while (1) {
        char ch = 1;
        bytes_read = read(fd[0], &ch, 1);
        if(bytes_read == 0) break;
        write(fd[1], &ch, 1);
    }
}
