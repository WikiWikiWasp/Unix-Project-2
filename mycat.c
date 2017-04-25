#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {

  char *filename;
  char buffer[BUFSIZ];
  char ch;
  struct stat pathStat;
  FILE *fp;

  if (argc > 1) {                  // if arguments are supplied, cat each one
    int i=1;
    for (;i<argc; i++) {           // loop through the args
      filename = argv[i];
      stat(filename, &pathStat);   // get file info
      fp = fopen(filename, "r");   // open file
      if (fp == NULL) {            // throw error if not found
        printf("mycat: %s: No such file or directory",filename);
        exit(1);
      }
      if (S_ISDIR(pathStat.st_mode)) {                  // if dir, not file
        printf("mycat: %s: Is a directory\n",filename); // print "is a dir"
      }
      while (fgets(buffer, BUFSIZ, fp) != NULL) {       // print file contents
        printf("%s",buffer);
      }
    }
  } else {
      // if no args detected, handle the cases ./mycat <foo.txt & mycat $(myls)
      while (read(STDIN_FILENO, &ch, 1) != 0) {
        if (ch == ' ') {   // manually add line breaks for mycat $(myls) case
          printf("\n");
        } else {           // print each char in case of ./mycat <foo.txt
          printf("%c",ch);
        }
      }
  }

  return 0;
}
