#include "bg.cpp"
#include "cd.cpp"
#include "echo.cpp"
#include "ls.cpp"
#include "pinfo.cpp"
#include "prompt.cpp"
#include "search.cpp"
#include <climits>
#include <dirent.h>
#include <grp.h>
#include <iostream>
#include <pwd.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

using namespace std;
using namespace filesystem;
#define CSIZE 2048
#define COMMANDNO 64

int fgid = 0;

void handleZ(int sig) {
  if (fgid == getpid()) {
    return;
  }
  kill(fgid, SIGSTOP);
}
void handleC(int sig) {
  if (fgid == getpid()) {
    return;
  }
  kill(fgid, SIGINT);
}


int main() {
  vector<string> history(20);
  int histpointer = 0;

  signal(SIGTSTP, handleZ);
  signal(SIGINT, handleC);
  
  char temphome[PATH_MAX];
  setenv("PWD", getcwd(temphome, sizeof(temphome)), 1);
  setenv("HOME", getcwd(temphome, sizeof(temphome)), 1);
  string lastdir = getenv("HOME");
  char *args[COMMANDNO];
  char hostname[HOST_NAME_MAX];

  int background = 0;

  char *home_directory = getenv("HOME");
  if (home_directory != NULL) {
    chdir(home_directory);
  }

  while (1) {
    display_prompt();

    // Take input and tokenize
    //=======================================================================================

    string command;
    if (!getline(cin, command)) {
      if (cin.eof()) {
        cout<<endl;
        break;
      }
    }
    history[histpointer++ % 20] = command; 
    //getline(cin, command);

    istringstream tempstream(command);
    vector<string> args1;
    string arg;
    while (tempstream >> arg) {
      args1.push_back(arg);
    }
    int arg_count = args1.size();
    if (arg_count == 0) {
      continue;
    }

    char *args[COMMANDNO] = {nullptr};
    for (size_t i = 0; i < arg_count; i++) {
      args[i] = const_cast<char *>(args1[i].c_str());
    }

    ostringstream output_stream;

    // Check for input redirection
    //=======================================================================================

    int indexOfGreaterThan = -1;
    for (int i = 0; i < arg_count; i++) {
      if (strcmp(args[i], "<") == 0) {
        indexOfGreaterThan = i;
        break;
      }
    }
    if (indexOfGreaterThan != -1) {
      int i = indexOfGreaterThan;
      for (; i < arg_count ; i++) {
        args[i] = args[i + 1];
      }
      args[i] = nullptr;
      arg_count--;
    }

    // Check for bg
    //=======================================================================================

    if (strcmp(args[arg_count - 1], "&") == 0) {
      args[arg_count - 1] = nullptr;
      dobgstuff(args);
      continue;
    }

    // Check for pipe
    //=======================================================================================
    int first = 1;
    int start = 0;
    int pflag = 0;
    int rflag = 0;

    int commandover = 0;

    for (size_t i = 0; i < arg_count; i++) {
      if (strcmp(args[i], "|") == 0) {
        pflag = 1;
      }
    }
    string fileContents;
    if (pflag == 1)
      for (size_t i = 0; i <= arg_count; i++) {
        if (strcmp(args[i], "|") == 0 && first == 1) {
          args[i] = nullptr;
          args[arg_count] = (char *)"|";
          doexecvpstuffandout(args, output_stream,fgid);
          fileContents =  output_stream.str();
          first = 0;
          start = i + 1;
        } else if (strcmp(args[i], "|") == 0 && first == 0) {
          args[i] = nullptr;

          fileContents =  output_stream.str();
          istringstream input_stream(fileContents);
          
          indoexecvpstuffandout(args + start, input_stream, output_stream);
          fileContents =  output_stream.str();
          start = i + 1;
        } else if (strcmp(args[i], ">") == 0 && first == 0) {
          if (i + 1 < arg_count) {

            args[i] = nullptr;
            const char *outputFileName = args[i + 1];
            args[i + 1] = nullptr;

            ofstream outputFile(outputFileName);
            if (outputFile.is_open()) {

              outputFile << output_stream.str();
              outputFile.close();
              commandover = 1;
            } else {
              cout << "file does not exist" << endl;
            }
          }
        } else if (strcmp(args[i], ">>") == 0 && first == 0) {
          if (i + 1 < arg_count) {

            args[i] = nullptr;
            const char *outputFileName = args[i + 1];
            args[i + 1] = nullptr;

            ofstream outputFile(outputFileName, ios::app);
            if (outputFile.is_open()) {

              outputFile << output_stream.str();
              outputFile.close();
              commandover = 1;
            } else {
              cerr << "file does not exist" << endl;
            }
          }
        }
        if (commandover) {
          break;
        }
      }

    if (commandover) {
      commandover = 0;
      continue;
    }
    if (pflag == 1){
      string output_string1 = output_stream.str();
      cout<< output_string1;
      continue;
    }

    // Check for background execution
    //=======================================================================================
    
    if (strcmp(args[arg_count - 1], "&") == 0) {
      background = 1;
      args[arg_count - 1] = NULL;
    } else {
      background = 0;
    }

    // Check for output redirection
    //=======================================================================================
    bool output_redirected = false;
    bool append_output = false;
    string output_file;
    for (int i = 0; i < arg_count; ++i) {
      if (strcmp(args[i], ">") == 0) {
        output_redirected = true;
        output_file = args[i + 1];
        args[i] = NULL;
        args[i + 1] = NULL;
        break;
      } else if (strcmp(args[i], ">>") == 0) {
        output_redirected = true;
        append_output = true;
        output_file = args[i + 1];
        args[i] = NULL;
        args[i + 1] = NULL;
        break;
      }
    }
    if (output_redirected) {
      ofstream output_fstream;
      outputredirectionstore(args, arg_count, output_file, append_output,
                             output_fstream,fgid);
      continue;
    }

    // Finally non redirected normal commands
    //=======================================================================================
    if (strcmp(args[0], "exit") == 0) {
      exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
      docdstuff(args[1], lastdir);
    } else if (strcmp(args[0], "pwd") == 0) {
      dopwdstuff(cout);
    } else if (strcmp(args[0], "echo") == 0) {
      doechostuff(args + 1, arg_count - 1, cout);
    } else if (strcmp(args[0], "ls") == 0) {
      dolsstuff(args + 1, arg_count - 1, cout);
    } else if (strcmp(args[arg_count - 1], "&") == 0) {
      dobgstuff(args);
    } else if (strcmp(args[0], "pinfo") == 0 && arg_count == 2) {
      cout<<"args here";
      int pid = stoi(args[1]);
      dopinfostuff(pid);
    }else if (strcmp(args[0], "pinfo") == 0) {
      int pid = getpid();
      dopinfostuff(pid);
    } else if (strcmp(args[0], "history") == 0 && arg_count > 1) {
      int startpoint =( histpointer - stoi(args[1]) + 20 ) % 20;
      int i = 0;
      while(i<stoi(args[1])){
        if(history[(startpoint +i) % 20].size()>0){
        cout<<history[(startpoint +i) % 20]<<endl;}
        i++;
      }
    } else if (strcmp(args[0], "history") == 0 ) {
      for(int i = histpointer+1 ; (i % 20) != histpointer ; i++ ){
        if(history[i % 20].size()>0)
        cout<<history[i % 20]<<endl;
      }
    }else if (strcmp(args[0], "search") == 0) {
      path currentPath = current_path();
      cout << searchFile(currentPath, args[1])<<endl;
    } else {
      doexecvpstuffandout(args, cout,fgid);

    }
  }

  return 0;
}
