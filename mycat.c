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

  if (argc > 1) {
    int i=1;
    for (;i<argc; i++) {
      filename = argv[i];
      stat(filename, &pathStat);
      fp = fopen(filename, "r");
      if (fp == NULL) {
        printf("mycat: %s: No such file or directory",filename);
        exit(1);
      }
      if (S_ISDIR(pathStat.st_mode)) {
        printf("mycat: %s: Is a directory\n",filename);
      }
      while (fgets(buffer, BUFSIZ, fp) != NULL) {
        printf("%s",buffer);
      };
    }
  } else {
      while (read(STDIN_FILENO, &ch, 1) != 0) {
        if (ch == ' ') {
          printf("\n");
        } else {
          printf("%c",ch);
        }
      }
  }

  return 0;
}
