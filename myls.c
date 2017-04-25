#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>


/* prints details for one file */
/* takes file name string param */
int printFileDetail(char *file)
{
  struct passwd *user;
  struct group *grp;
  struct stat sb;
  char   mTimeStr[21];

  if (stat(file, &sb) == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  }

  user = getpwuid(sb.st_uid);
  grp = getgrgid(sb.st_gid);
  strftime(mTimeStr, sizeof(mTimeStr), " %I:%M:%S, %m/%d/%Y", localtime(&sb.st_mtime));

  /* http://stackoverflow.com/a/10323131/2446454 */
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

  printf(" %d %s %s %lld, %s%s",
    sb.st_nlink,
    user->pw_name,
    grp->gr_name,
    (long long) sb.st_size,
    mTimeStr,
    file
  );

  return 0;
}






void printDir(char *path) {
  struct dirent *namelist;
  DIR *dir;
  int n,
      i=0;

  dir = opendir(path);
  if (dir) {
    while ((namelist = readdir(dir)) != NULL) {
      printf("%s\n", namelist->d_name);
    }
    closedir(dir);
  } else {
    printf("directory %s not found\n",path);
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

  //printf ("lflag = %d\n", lflag);

  struct stat path_stat;
  stat(argv[optind], &path_stat);

  if (!argv[optind]) {        // if no args supplied, list files in current dir
    printDir(".");
  } else {                // if args supplied, check if source is file or dir

    if (S_ISREG(path_stat.st_mode) == 1) {         // print file
      if (lflag) {
        printFileDetail(argv[optind]);
      } else {
        printf("%s",argv[optind]);
      }
    } else if (S_ISDIR(path_stat.st_mode) == 1) {  // print dir
      printDir(argv[optind]);
      // printDir(file_stat, file_name, long_flag);
    } else {
      printf("No file or directory %s found",argv[optind]);
    }

  }

  return 0;
}
