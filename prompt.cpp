#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <iostream>


using namespace std;


void display_prompt() {
    char cwd[8192];
    char hostname[HOST_NAME_MAX];
    cout<< getenv("USER")<<"@";
    gethostname(hostname, HOST_NAME_MAX);
    cout<<hostname;
    char *homepath = getenv("HOME");
    getcwd(cwd, sizeof(cwd));

    if (strstr(cwd, homepath) == cwd) {
        if (strlen(cwd) == strlen(homepath)) {
            cout << "~";  
        } else {
            cout << "~" << (cwd + strlen(homepath) + 1); 
        }
    } else {
        printf("%s", cwd);
    }

   cout<<">";
}
