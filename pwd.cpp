#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
using namespace std;

void dopwdstuff(ostream &output_stream) {
  char chunk[8192];
  getcwd(chunk, sizeof(chunk));
  output_stream << chunk << endl;
  
}


string quickpwd() {
  char chunk[8192];
  getcwd(chunk, sizeof(chunk));
  string ret(chunk);
  return ret;
  
}