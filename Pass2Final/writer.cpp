#include "writer.h"
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

void writer::createFile(string name){
    char location[100];
    strcpy(location, name.c_str());
    myfile.open(location);
}

void writer::writeToFile(string line){
    myfile<<line<<endl;
}
