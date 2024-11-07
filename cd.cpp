#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <string>

using namespace std;

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "pwd.cpp"

void docdstuff(const char *args,string & lastdir) {
    {
        char current[2048];
        const char* home_dir;
        string currdir;
        if (args == NULL){
            lastdir = quickpwd();
            chdir( getenv("HOME"));
            return;
        }
        switch (args[0]) {
            case '\0':  lastdir = quickpwd();
                        chdir( getenv("HOME"));break;
            
            case '-':   currdir = quickpwd();
                        chdir(lastdir.c_str());
                        lastdir = currdir;
                        break;

            case '~':   lastdir = quickpwd();
                        chdir( getenv("HOME"));break;
            

            default:lastdir = quickpwd(); chdir(args); break;
        }

        
    }
}

