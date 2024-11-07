


#include <climits>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <grp.h>
#include <iostream>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;



void doexecvpstuffandout(char *args[], std::ostream& output_stream,int fgid) {
    int pipefd[2];
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }
    
    pid_t child_pid = fork();
    
    if (child_pid == -1) {
        perror("fork");
        return;
    }
    
    if (child_pid == 0) {
        fgid = child_pid;
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]);
        
        
        execvp(args[0], args);
        perror("execvp"); 
        exit(0);
        
    } else {

        close(pipefd[1]); 
        
        char buffer[4096];
        
        ssize_t n;

        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output_stream.write(buffer, n); 
        }

        
        close(pipefd[0]); 
        
        
        int status;
        waitpid(child_pid, &status, 0);
    }
}


void indoexecvpstuffandout(char *args[], std::istream &input_stream, std::ostream &output_stream) {
    int pipefd_input[2];
    int pipefd_output[2];

    if (pipe(pipefd_input) == -1 || pipe(pipefd_output) == -1) {
        perror("pipe");
        return;
    }

    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        return;
    }

    if (child_pid == 0) {
        
        close(pipefd_input[1]); 
        dup2(pipefd_input[0], STDIN_FILENO); 
        close(pipefd_input[0]); 

        close(pipefd_output[0]); 
        dup2(pipefd_output[1], STDOUT_FILENO); 
        close(pipefd_output[1]);

       
        execvp(args[0], args);
        perror("execvp"); 
        exit(0);
        return;
    } else {
        
        close(pipefd_input[0]); 
        close(pipefd_output[1]);

        
        char chunk[8192];
        while (input_stream.read(chunk, sizeof(chunk))) {
            ssize_t bytesRead = input_stream.gcount();
            write(pipefd_input[1], chunk, bytesRead);
        }
        close(pipefd_input[1]); 

        
        while (true) {
            ssize_t n = read(pipefd_output[0], chunk, sizeof(chunk));
            if (n <= 0) {
                break; 
            }
            output_stream.write(chunk, n); 
        }
        close(pipefd_output[0]); 

      
        int status;
        waitpid(child_pid, &status, 0);

       return;
    }
}





void outputredirectionstore(char *args[], int arg_count, string output_file,int append_output,ofstream &output_stream,int &fgid) {
  
  if (append_output) 
  {
    output_stream.open (output_file, ios::out  | ios::app);
  } else 
  {
    output_stream.open (output_file);
  }
  if(!output_stream){
    cout<<"file does not exist";
    return;
  }

  streambuf *savethecout = cout.rdbuf();

  cout.rdbuf(output_stream.rdbuf());

  doexecvpstuffandout(args,cout,fgid);

  cout.rdbuf(savethecout);
  
}

