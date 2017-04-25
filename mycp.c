#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

/*******************************************************************
* NAME :            fileCopy
*
* DESCRIPTION :     copies a source file to destination file
*
* INPUTS :
*       PARAMETERS:
*           char    *source        source file
*           char    *dest          destination file
*******************************************************************/

void fileCopy(char *source, char *dest) {

  char buffer[BUFSIZ];
  FILE *sourceFile,
       *destFile;
  size_t readBytes;

  sourceFile = fopen(source, "r");        // open source file
  destFile = fopen(dest, "w");            // open dest file

  // copy contents of sourceFile to destFile
  while (0 < (readBytes = fread(buffer, 1, BUFSIZ, sourceFile))) {
    fwrite(buffer, 1, readBytes, destFile);
  }

}

int main (int argc, char **argv)
{

  int    c;
  int    index;
  int    sourceLen;
  int    destLen;
  int    aflag = 0;
  int    rflag = 0;
  char   *rvalue = NULL;
  char   *source;
  char   *dest;
  char   *sourcePath;
  char   *destPath;
  DIR		 *sourceDirPtr;
  struct stat pathStat;
  struct dirent	*fileInDir;
  struct stat destDir = {0};

  opterr = 0;

  while ((c = getopt (argc, argv, "R:")) != -1)
    switch (c)
      {
      case 'R':
        rflag = 1;
        rvalue = optarg;
        break;
      case '?':
        if (optopt == 'R')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option '-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character '\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  if (!rflag) {
    if (argc < 3) {
      printf("usage: mycp <source> <destination>");
      return 1;
    }
    source = argv[1];           // set source to 2nd arg
    dest = argv[2];             // set dest to 3rd arg
  } else {
    if (argc < 4) {
      printf("usage: mycp <source> <destination>");
      return 1;
    }
    source = argv[2];           // set source to 2nd arg
    dest = argv[3];             // set dest to 3rd arg
  }

  stat(source, &pathStat);    // get pathStat file details

  if (S_ISREG(pathStat.st_mode)) {                                   // if file
    fileCopy(source, dest);
  } else if (S_ISDIR(pathStat.st_mode)) {                            // if directory
    if (!rflag) {
      fprintf(stderr,"mycp: %s is a directory (not copied).\n",source);// error
    } else {

      if ((sourceDirPtr = opendir(source)) == NULL) {                // open dir
    		fprintf(stderr,"directory not found");
      }

      if (stat(dest, &destDir) == -1) {                             // create dir
        mkdir(dest, 0755);
      }

      while ((fileInDir = readdir(sourceDirPtr)) != NULL) {         // for each file
        sourceLen = sizeof(source) + sizeof(fileInDir->d_name) + 1; // source len
        destLen = sizeof(dest) + sizeof(fileInDir->d_name) + 1;     // dest len
        sourcePath = malloc(sourceLen);
        destPath = malloc(destLen);
        strcpy(sourcePath, source);                                 // set src path
        strcat(sourcePath,"/");
        strcat(sourcePath,fileInDir->d_name);
        strcpy(destPath, dest);                                     // set dest path
        strcat(destPath,"/");
        strcat(destPath,fileInDir->d_name);
        fileCopy(sourcePath, destPath);                             // copy file
        free(sourcePath);                                           // free src path
        free(destPath);                                             // free dest path
      }

      closedir(sourceDirPtr);                                       // close dir

    }
  }

  return 0;
}
