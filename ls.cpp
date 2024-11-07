#include <cstring>
#include <dirent.h>
#include <grp.h>
#include <iostream>
#include <pwd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <vector>

using namespace std;

void list_files(const char *path, int amode, int longmode, ostream &output) {
  DIR *drectry = opendir(path);
  if (drectry == NULL) {
    perror("opendir");
    return;
  }

  struct dirent *eachofthefile;
  while ((eachofthefile = readdir(drectry)) != NULL) {
    if (amode == 0) {
      if (eachofthefile->d_name[0] == '.')
        continue;
    }

    if (longmode == 1) {
      struct stat file_information;
      char full_path[2048];
      snprintf(full_path, sizeof(full_path), "%s/%s", path,
               eachofthefile->d_name);

      if (stat(full_path, &file_information) == 0) {

        struct passwd *pass = getpwuid(file_information.st_uid);

        struct group *gr = getgrgid(file_information.st_gid);

        char filerights[11];

        if (S_ISDIR(file_information.st_mode)) {
          filerights[0] = 'd';
        } else {
          filerights[0] = '-';
        }
        if (file_information.st_mode & S_IRUSR) {
          filerights[1] = 'r';
        } else {
          filerights[1] = '-';
        }
        if (file_information.st_mode & S_IWUSR) {
          filerights[2] = 'w';
        } else {
          filerights[2] = '-';
        }
        if (file_information.st_mode & S_IXUSR) {
          filerights[3] = 'x';
        } else {
          filerights[3] = '-';
        }
        if (file_information.st_mode & S_IRGRP) {
          filerights[4] = 'r';
        } else {
          filerights[4] = '-';
        }
        if (file_information.st_mode & S_IWGRP) {
          filerights[5] = 'w';
        } else {
          filerights[5] = '-';
        }
        if (file_information.st_mode & S_IXGRP) {
          filerights[6] = 'x';
        } else {
          filerights[6] = '-';
        }
        if (file_information.st_mode & S_IROTH) {
          filerights[7] = 'r';
        } else {
          filerights[7] = '-';
        }
        if (file_information.st_mode & S_IWOTH) {
          filerights[8] = 'w';
        } else {
          filerights[8] = '-';
        }
        if (file_information.st_mode & S_IXOTH) {
          filerights[9] = 'x';
        } else {
          filerights[9] = '-';
        }
        filerights[10] = '\0';

        char strtime[128];
        struct tm *timechunk = localtime(&file_information.st_mtime);

        strftime(strtime, sizeof(strtime), "%b %d %H:%M", timechunk);

        output << filerights << "\t" << file_information.st_nlink << "\t"
               << pass->pw_name << "\t" << gr->gr_name << "\t"
               << file_information.st_size << "\t" << strtime << "\t"
               << eachofthefile->d_name << endl;
      }
    } else {
      output << eachofthefile->d_name << endl;
    }
  }

  closedir(drectry);
}
void display_file_info(const char *filename, int amode, int longmode,
                       ostream &output) {

  if (!longmode) {
    output << filename << endl;
    return;
  }
  struct stat file_information;

  if (stat(filename, &file_information) == 0) {
    if (amode == 0) {
      if (filename[0] == '.')
        return;
    }

    struct passwd *pass = getpwuid(file_information.st_uid);
    struct group *gr = getgrgid(file_information.st_gid);

    char filerights[11];
    if (S_ISDIR(file_information.st_mode)) {
      filerights[0] = 'd';
    } else {
      filerights[0] = '-';
    }
    if (file_information.st_mode & S_IRUSR) {
      filerights[1] = 'r';
    } else {
      filerights[1] = '-';
    }
    if (file_information.st_mode & S_IWUSR) {
      filerights[2] = 'w';
    } else {
      filerights[2] = '-';
    }
    if (file_information.st_mode & S_IXUSR) {
      filerights[3] = 'x';
    } else {
      filerights[3] = '-';
    }
    if (file_information.st_mode & S_IRGRP) {
      filerights[4] = 'r';
    } else {
      filerights[4] = '-';
    }
    if (file_information.st_mode & S_IWGRP) {
      filerights[5] = 'w';
    } else {
      filerights[5] = '-';
    }
    if (file_information.st_mode & S_IXGRP) {
      filerights[6] = 'x';
    } else {
      filerights[6] = '-';
    }
    if (file_information.st_mode & S_IROTH) {
      filerights[7] = 'r';
    } else {
      filerights[7] = '-';
    }
    if (file_information.st_mode & S_IWOTH) {
      filerights[8] = 'w';
    } else {
      filerights[8] = '-';
    }
    if (file_information.st_mode & S_IXOTH) {
      filerights[9] = 'x';
    } else {
      filerights[9] = '-';
    }
    filerights[10] = '\0';

    char strtime[64];
    struct tm *timechunk = localtime(&file_information.st_mtime);
    strftime(strtime, sizeof(strtime), "%b %d %H:%M", timechunk);

    output << filerights << "\t" << file_information.st_nlink << "\t"
           << pass->pw_name << "\t" << gr->gr_name << "\t"
           << file_information.st_size << "\t" << strtime << "\t" << filename
           << endl;
  } else {
    perror("stat");
  }
}

void dolsstuff(char *args[], int arg_count, ostream &output) {
  int amode = 0, longmode = 0;
  bool path_specified = false;
  vector<string> filenames;

  for (int i = 0; i < arg_count; i++) {
    if (args[i][0] == '-') {

      if (strcmp(args[i], "-la") == 0 || (strcmp(args[i], "-al") == 0)) {
        amode = 1;
        longmode = 1;
      }
      if (strcmp(args[i], "-a") == 0) {
        amode = 1;
      }
      if (strcmp(args[i], "-l") == 0) {
        longmode = 1;
      }

    } else {

      filenames.push_back(args[i]);
      path_specified = true;
    }
  }

  if (!path_specified) {
    list_files(".", amode, longmode, output);
  } else {

    for (string filename : filenames) {
      if (filename.find('.') != string::npos)
        display_file_info(filename.c_str(), amode, longmode, output);
      else {
        list_files(filename.c_str(), amode, longmode, output);
      }
    }
  }
}
