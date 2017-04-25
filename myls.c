nclude <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <pwd.h>
#include <grp.h>


// get file size in bytes
// takes in filename in char array, returns the file size (in bytes)
long long getFileSize(char *file)
{
  struct stat sb;
  long long filesize;

  if (stat(file, &sb) == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  }
  filesize = (long long)sb.st_size;

  return filesize;
}

// prints details for one file
// takes file name string param
int printFileDetail(char *file, int widthSize)
{
  struct passwd *user;
  struct group *grp;
  struct stat sb;
  char   mTimeStr[21];

  if (stat(file, &sb) == -1) {  // get file info
    perror("stat");
    exit(EXIT_FAILURE);
  }

  user = getpwuid(sb.st_uid);   // get user id
  grp = getgrgid(sb.st_gid);    // get group id

  // get date & time of last file modification
  strftime(mTimeStr, sizeof(mTimeStr), " %b %d %I:%M", localtime(&sb.st_mtime));

  // file permissions output from http://stackoverflow.com/a/10323131/2446454
  printf( (S_ISDIR(sb.st_mode)) ? "d" : "-");
  printf( (sb.st_mode & S_IRUSR) ? "r" : "-");
  printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
  printf( (sb.st_mode & S_IXUSR) ? "x" : "-");
  printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
  printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
  printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
  printf( (sb.st_mode & S_IROTH) ? "r" : "-");
  printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
  printf( (sb.st_mode & S_IXOTH) ? "x" : "-");

  // print number of links, user name, group name
  //printf(" %d %s %s ", sb.st_nlink, user->pw_name, grp->gr_name); // mac osx
  printf(" %lu %s %s ", sb.st_nlink, user->pw_name, grp->gr_name); // ubuntu

  // print file size (width formatted to longest filesize)
  printf("%*lld", widthSize, (long long) sb.st_size);
  printf("%s %s\n", mTimeStr, file); // print date & time of last file mod

  return 0;
}





// print directory
// take in directory name in char array
void printDir(char *path) {
  struct dirent *namelist;
  DIR *dir;
  int n,
      i=0;

  dir = opendir(path);
  if (dir) {
    while ((namelist = readdir(dir)) != NULL) {
      if (strcmp(namelist->d_name,".") != 0 && strcmp(namelist->d_name,"..") != 0) {
        printf("%s ", namelist->d_name);
      }
    }
    closedir(dir);
  } else {
    printf("directory %s not found\n",path);
  }
  free(namelist);
}

void printDirDetails(char *path) {
  struct dirent *namelist;
  DIR *dir;
  int n,
      widthDigits,
      i=0;
  long long largestFileSize = 0;

  dir = opendir(path);
  if (dir) {

    // get the width of the digits of the largest file size
    while ((namelist = readdir(dir)) != NULL) {
      if (strcmp(namelist->d_name,".") != 0 && strcmp(namelist->d_name,"..") != 0) {
        if (getFileSize(namelist->d_name) > largestFileSize) {
          largestFileSize = getFileSize(namelist->d_name);
        }
      }
    }
    widthDigits = (int)floor(log10(llabs(largestFileSize))) + 1;
  } else {
    printf("directory %s not found\n",path);
  }

  closedir(dir);
  dir = opendir(path);
  if (dir) {
    // print all the file details
    while ((namelist = readdir(dir)) != NULL) {

      if (strcmp(namelist->d_name,".") != 0 && strcmp(namelist->d_name,"..") != 0) {
        printFileDetail(namelist->d_name, widthDigits);
      }
    }
    closedir(dir);
  }

  free(namelist);
}



int main(int argc, char *argv[]) {

  int lflag = 0;

  char *cvalue = NULL;
  int index;
  int c;

  opterr = 0;

  while ((c = getopt (argc, argv, "l")) != -1)
    switch (c)
      {
      case 'l':
        lflag = 1;
        break;
      case '?':
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  struct stat path_stat;
  stat(argv[optind], &path_stat);

  if (!argv[optind]) {        // if no args supplied, list files in current dir
    if (lflag) {
      printDirDetails(".");
    } else {
      printDir(".");
    }

  } else {                // if args supplied, check if source is file or dir

    if (S_ISREG(path_stat.st_mode) == 1) {         // print file
      int widthDigits;
      if (lflag) {
        widthDigits = (int)floor(log10(llabs(getFileSize(argv[optind])))) + 1;;
        printFileDetail(argv[optind], widthDigits);
      } else {
        printf("%s",argv[optind]);
      }
    } else if (S_ISDIR(path_stat.st_mode) == 1) {  // print dir
      printDir(argv[optind]);
    } else {
      printf("No file or directory %s found",argv[optind]);
    }

  }

  return 0;
}
