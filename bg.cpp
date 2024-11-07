#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "doredirectcommands.cpp"
using namespace std;



void dobgstuff(char *args[]) {
    int pid = fork();
    if(pid == 0){
        ostringstream output_stream;
        doexecvpstuffandout(args,output_stream,0);
        exit(0);
    }
    else{
        cout<<pid<<endl;
        return;
    }
}
