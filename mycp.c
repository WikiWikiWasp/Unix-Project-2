#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


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

int main(int argc, char *argv[]) {

  int sourceLen;
  int destLen;
  char *source;
  char *dest;
  char *sourcePath;
  char *destPath;
  DIR		*sourceDirPtr;
  struct stat pathStat;
  struct dirent	*fileInDir;
  struct stat destDir = {0};

  if (argc < 3) {
    printf("usage: mycp <source> <destination>");
    return 1;
  }

  source = argv[1];           // set source to 2nd arg
  dest = argv[2];             // set dest to 3rd arg
  stat(source, &pathStat);    // get pathStat file details

  if (S_ISREG(pathStat.st_mode)) {          // if file

    fileCopy(source, dest);                 // copy file

  } else if (S_ISDIR(pathStat.st_mode)) {   // if directory

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



  return 0;
}
