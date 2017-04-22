#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_SIZE 100

int main(int argc, char** argv) {
  while (1) {
    pid_t pid;
    int status;
    char a[100]; 
    char cmd[100]; 

    pid = fork();
    if (pid < 0) {
      printf("fail\n"); return 1;
    }
    else if (pid == 0) {
        printf(">");
	fgets(a, MAX_SIZE, stdin);
	
	if ((strlen(a)>0) && (a[strlen(a) -1] == '\n'))
	  a[strlen(a) - 1] = '\0';
	
	char ** res  = NULL;
	char *  p    = strtok (a, " ");
	int n_spaces = 0, i;

	while (p) {
	  res = realloc (res, sizeof (char*) * ++n_spaces);
	  
	  if (res == NULL)
	    exit (-1); 

	  res[n_spaces-1] = p;

	  p = strtok (NULL, " ");
	}

	res = realloc (res, sizeof (char*) * (n_spaces+1));
	res[n_spaces] = 0;
 
	strcpy(cmd, res[0]);
	execvp(cmd, res);
	printf("%s\n", strerror(errno)); 
	exit(errno); 
      } 
  
      else if (pid > 0) {
	waitpid(pid, &status, 0);          
    }   
  }
}
