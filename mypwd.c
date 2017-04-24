#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argv, char **argc)
{
   char cwd[1024];

//   getcwd (cwd, sizeof(cwd));

   printf ("%s\n", getcwd (cwd, sizeof(cwd)));

   return 0;
}
