#include <ratio>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace std;


void doechostuff(char *args[], int arg_count, std::ostream &output_stream) {
    for (int i = 0; i < arg_count; i++) {
        output_stream << args[i];
        if (i < arg_count - 1) {
            output_stream << " ";
        }
    }
    output_stream << std::endl;
}
