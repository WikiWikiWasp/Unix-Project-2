#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

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

// print simple directory
// take in directory name in char array
// prints out just names of files and directories
void printDir(char *path) {
  // struct dirent *namelist;
  struct dirent **namelist;
  DIR *dir;
  struct winsize w;
  int n,
      i=0,
      colCount = 0,
      largestFilename = 0;

  // first time loop through each file and get longest filename
  dir = opendir(path);                             // open directory
  if (dir) {
    n = scandir(path, &namelist, NULL, alphasort); // read files except . & ..
    if (n < 0)
        perror("scandir error");
    else {
      while (i<n) {
        if (strcmp(namelist[i]->d_name,".") != 0 && strcmp(namelist[i]->d_name,"..") != 0) {
          if (strlen(namelist[i]->d_name) > largestFilename) {
            largestFilename = strlen(namelist[i]->d_name);
          }
        }
        free(namelist[i]);
        ++i;
      }
    }
    closedir(dir);                                 // close dir
  } else {
    printf("directory %s not found\n",path);       // print err if dir not found
  }

  // second time loop through each file and print filename
  dir = opendir(path);                             // open directory
  if (dir) {
    n = scandir(path, &namelist, NULL, alphasort); // read files except . & ..
    if (n < 0)
        perror("scandir error");
    else {
      i=0;
      while (i<n) {
        // print each filename width width equal to longest filename width
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);      // get terminal width
        if (strcmp(namelist[i]->d_name,".") != 0 && strcmp(namelist[i]->d_name,"..") != 0) {
          printf("%-*s ", largestFilename, namelist[i]->d_name);
          colCount += largestFilename;
          // add linebreak on every column closest to terminal's right edge
          if ( (colCount + largestFilename) > (w.ws_col) ) {
            printf("\n");  // add line break
            colCount = 0;  // reset character column colunt
          }
        }
        free(namelist[i]);
        ++i;
      }
    }
    closedir(dir);                                 // close dir
  }

  free(namelist);                                  // deallocate namelist
}

// print detailed directory
// take in directory name in char array
// prints out names of files and directories with all file details
void printDirDetails(char *path) {
  struct dirent *namelistPtr;
  struct dirent **namelist;
  DIR *dir;
  int n,
      widthDigits,
      i=0;
  long long largestFileSize = 0;

  // first time, just gets width of digits of largest file size's bytes
  dir = opendir(path);                            // open dir
  if (dir) {
    // get the width of the digits of the largest file size
    while ((namelistPtr = readdir(dir)) != NULL) {
      if (strcmp(namelistPtr->d_name,".") != 0 && strcmp(namelistPtr->d_name,"..") != 0) {
        if (getFileSize(namelistPtr->d_name) > largestFileSize) {
          largestFileSize = getFileSize(namelistPtr->d_name);
        }
      }
    }
    widthDigits = (int)floor(log10(llabs(largestFileSize))) + 1;
  } else {
    printf("directory %s not found\n",path);     // print error if not found
  }
  closedir(dir);                                 // close directory

  // second time, prints details of every file
  dir = opendir(path);
  if (dir) {
    n = scandir(path, &namelist, NULL, alphasort); // read files except . & ..
    if (n < 0)
        perror("scandir error");
    else {
      while (i<n) {
        if (strcmp(namelist[i]->d_name,".") != 0 && strcmp(namelist[i]->d_name,"..") != 0) {
          printFileDetail(namelist[i]->d_name, widthDigits);  // print file details
        }
        free(namelist[i]);
        ++i;
      }
    }
    closedir(dir);                              // close directory
  }
  free(namelist);                               // deallocate namelist
}

int main(int argc, char *argv[]) {

  char *cvalue = NULL;
  int index;
  int c;
  int lflag = 0;

  opterr = 0;                                   // for getopt

  while ((c = getopt (argc, argv, "l")) != -1)  // loop through options
    switch (c)
      {
      case 'l':                                 // if -l option, set lflat to 1
        lflag = 1;
        break;
      case '?':                                 // all other options throw err
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
  stat(argv[optind], &path_stat);               // get directory details

  if (!argv[optind]) {        // if no args supplied, list files in current dir
    if (lflag) {
      printDirDetails(".");   // if -l option, print filenames with details
    } else {
      printDir(".");          // if no -l option, print simple filenames
    }

  } else {                // if args supplied, check if source is file or dir

    if (S_ISREG(path_stat.st_mode) == 1) {         // if file, not directory
      int widthDigits;
      if (lflag) {                                 // if -l
        // get width of digits of filesize's bytes
        widthDigits = (int)floor(log10(llabs(getFileSize(argv[optind])))) + 1;
        printFileDetail(argv[optind], widthDigits);// print file details
      } else {
        printf("%s",argv[optind]);                // if no -l print simple files
      }
    } else if (S_ISDIR(path_stat.st_mode) == 1) {  // if directory, not file
      printDir(argv[optind]);                      // print the directory
    } else {
      printf("No file or directory %s found\n",argv[optind]); // err if not found
    }

  }

  return 0;
}
