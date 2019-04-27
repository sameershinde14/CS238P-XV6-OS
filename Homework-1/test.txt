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
  	int src, dest, sz, szw; 
	char *c = (char *) calloc(100, sizeof(char)); 

	src = open(argv[1], O_RDONLY);
	dest = open(argv[2],  O_WRONLY | O_CREAT | O_TRUNC, 0644); 
	if (src < 0) { 
		perror("r1"); 
		exit(1); 
	} 

	do{
		sz = read(src, c, 10); 
		c[sz] = '\0'; 
		szw = write(dest, c, sz);  
	}while(sz!=0);
	
	close(src);
	close(dest);
 	exit(0);
}


