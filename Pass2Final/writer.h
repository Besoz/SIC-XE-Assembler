#ifndef WRITER_H_INCLUDED
#define WRITER_H_INCLUDED
#include <fstream>
#include <string>

using namespace std;

class writer{
public :
    ofstream myfile;
public :
  //  writer();
    void createFile(string name);
    void writeToFile(string line);
};

#endif // WRITER_H_INCLUDED
