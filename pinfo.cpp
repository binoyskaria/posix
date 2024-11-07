#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace std;

void dopinfostuff(int pid) {

  cout << "pid: " << pid << endl;

  // string statusvalueinFile = readstatusFileStream(pid);

  string status;
  string statusvalueinFile;

  string temp = to_string(pid);
  string statusPath = "/proc/" + temp + "/status";
  ifstream statusFileStream(statusPath);

 
  string line;

  while (getline(statusFileStream, line)) {
    int didifind = line.find("State:");
    if (didifind == 0) {
      statusvalueinFile = line.substr(7);
      break;
    }
  }

  statusFileStream.close();
  string processdiagstat = statusvalueinFile;
  if (processdiagstat[0] == 'R') {
    status = "R";
  } else if (processdiagstat[0] == 'S') {
    status = "S";
  } else if (processdiagstat[0] == 'Z') {
    status = "Z";
  } else if (processdiagstat[0] == 'T') {
    status = "T";
  } else {
    status = "Unknown";
  }
  cout << "Process Status:  " << status << endl;

  int memory = 0;

 
  ifstream statusFileStream2(statusPath);

  while (getline(statusFileStream2, line)) {
    if (line.find("VmSize:") == 0) {
      string memorynumber = line.substr(7);
      memory = atoi(memorynumber.c_str());
      break;
    }
  }
  statusFileStream2.close();
  cout << "Memory: " << memory << endl;

  char chunk[1024];
  string pathofprocess;
  string path = "/proc/" + temp + "/exe";
  int diditread = readlink(path.c_str(), chunk, sizeof(chunk) - 1);
  chunk[diditread] = '\0';
  pathofprocess = chunk;
  string executablePath = pathofprocess;
  cout << "Executable Path: " << executablePath << endl;
}
