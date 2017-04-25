#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_SIZE 100

int main(int argc, char** argv) {
  while (1) {
    pid_t pid;
    pid_t pipe_pid;
    int pipefd[2]; 
    int status;
    char a[100];
    char b[3];
    char c[2];
    char d[2];
    char e[2]; 
    strcpy(b, "my");
    strcpy(c, ">");
    strcpy(d, "<");
    strcpy(e, "|"); 
    char dotslash[3];
    strcpy(dotslash, "./"); 
    char cmd[100];
    char *holder1[2];
    char *holder2[2]; 
    int mystdout;
    int mystdin; 

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
	if (strncmp(cmd, b, strlen(b)) == 0) {
	  strcat(dotslash, cmd);
	  strcpy(cmd, dotslash);
	}

	if (res[1] != NULL) {
	  if (strncmp(res[1], d, strlen(d)) == 0) {
	    res[1]++;
	    mystdin = open(res[1], O_RDONLY); 
	    close(0);
	    dup2(mystdin, 0);
	  }
	  else if (strncmp(res[1], e, strlen(e)) == 0) {
	    pipe(pipefd);

	    pipe_pid = fork();

	    if (pipe_pid == 0) {
	      holder1[0] = cmd;
	      holder1[1] = NULL; 
	      dup2(pipefd[0], 0);
	      close(pipefd[1]);
	      execvp(cmd, holder1);
	      }
	    else {
	      strcpy(cmd, res[2]);
	      holder2[0] = cmd;
	      holder2[1] = NULL; 
	      dup2(pipefd[1], 1);
	      close(pipefd[0]);
	      execvp(cmd, holder2);
	      continue; 
	    }	  
	  }
	}

        if (res[2] != NULL) {
	  if (strncmp(res[2], c, strlen(c)) == 0) {
	    res[2]++;
	    mystdout = open(res[2], O_CREAT|O_WRONLY, 0666);
	    close(1);
	    dup2(mystdout, 1); 
	  }
	}
	  
	execvp(cmd, res);
	printf("%s\n", strerror(errno)); 
	exit(errno); 
      } 
  
      else if (pid > 0) {
	waitpid(pid, &status, 0);          
    }   
  }
}
