#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  char *filename = argv[1];
  char buffer[BUFSIZ];
  char ch;
  FILE *fp;

  if (argc > 1) {
    fp = fopen(filename, "r");
    while (fgets(buffer, BUFSIZ, fp) != NULL) {
      printf("%s",buffer);
    };
  } else {
    while (read(STDIN_FILENO, &ch, 1) != 0) {
      printf("%c",ch);
    }
  }

  return 0;
}
