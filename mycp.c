#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

/*******************************************************************
* NAME :            fileCopy
* DESCRIPTION :     copies a source file to destination file
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

/*******************************************************************
* NAME :            directoryCopy
* DESCRIPTION :     copies a source dir to destination dir
* INPUTS :
*       PARAMETERS:
*           char    *source        source directory
*           char    *dest          destination directory
*           int     rflag          -r option (0 or 1)
*******************************************************************/
void directoryCopy(char *source, char *dest, int rflag) {
  int    sourceLen;
  int    destLen;
  char   *sourcePath;
  char   *destPath;
  DIR		 *sourceDirPtr;
  struct dirent	*fileInDir;
  struct stat destDir = {0};

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

      struct stat fileStat;
      sourceLen = sizeof(source) + sizeof(fileInDir->d_name) + 1; // source len
      sourcePath = malloc(sourceLen);
      strcpy(sourcePath, source);                                 // set src path
      strcat(sourcePath,"/");
      strcat(sourcePath,fileInDir->d_name);
      destLen = sizeof(dest) + sizeof(fileInDir->d_name) + 1;     // dest len
      destPath = malloc(destLen);
      strcpy(destPath, dest);                                     // set dest path
      strcat(destPath,"/");
      strcat(destPath,fileInDir->d_name);

      // on file info success
      if (stat(sourcePath, &fileStat) == 0) {
        if (S_ISREG(fileStat.st_mode)) {
          fileCopy(sourcePath, destPath);
        } else if (S_ISDIR(fileStat.st_mode)) {
          if (strcmp(fileInDir->d_name,".") != 0 && strcmp(fileInDir->d_name,"..") != 0) {
            directoryCopy(sourcePath, destPath, rflag);
          }
        }
      } else {
        printf("stat error\n");
      }
      free(sourcePath);
      free(destPath);
    }
    closedir(sourceDirPtr);                                       // close dir
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

  if (S_ISREG(pathStat.st_mode)) {       // if file
    fileCopy(source, dest);
  } else if (S_ISDIR(pathStat.st_mode)) {// if dir, call directoryCopy recursively
    directoryCopy(source, dest, rflag);
  }

  return 0;
}
