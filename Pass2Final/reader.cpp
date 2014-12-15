#include "reader.h"
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

void reader::openFile(string name){
    char location[name.length()];
    strcpy(location, name.c_str());
    myfile.open(location);
};


string reader::getNext(){
    string line;
    getline(myfile,line);
    return line;
};

void reader::closeFile()
{
    myfile.close();
};
