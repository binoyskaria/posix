#include <filesystem>
#include <iostream>
#include <string>
using namespace std;
using namespace filesystem;

bool searchFile(const filesystem::path &currentDir, const string &tobefound) {
  int status = 0;
  if (!exists(currentDir)) {
    if (!is_directory(currentDir)) {
      cout << "Directory does not exist: " << endl;
      status = 0;
      return false;
    }
  }
  for (const auto &eachentry : directory_iterator(currentDir)) {
    const path &eachentryPath = eachentry.path();

    if (is_regular_file(eachentryPath) &&
        eachentryPath.filename() == tobefound) {
      cout << "Found the file flag :";
      status = 1;
      return true;
    } else if (is_directory(eachentryPath)) {
      if (searchFile(eachentryPath, tobefound)) {
        status = 0;
        return true;
      }
    }
  }
  return false;
}
