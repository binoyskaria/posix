#include "doredirectcommands.cpp"
#include <iostream>
#include <string>
#include <sstream>
using namespace  std;


int main() {
    char *args[1024];
    args[0] = (char*)"cat";
    args[1] = nullptr;

    std::ostringstream output_stream;
    std::string fileContents = "awwfvsda";
    std::istringstream input_stream(fileContents);

    indoexecvpstuffandout2(args, input_stream, output_stream);

    std::string output_string1 = output_stream.str();
    std::cout << output_string1;

    return 0;
}
