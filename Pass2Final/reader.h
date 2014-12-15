#ifndef READER_H_INCLUDED
#define READER_H_INCLUDED
#include <fstream>
#include <string>

using namespace std;

class reader{
public :
    ifstream myfile;
public :
    void openFile(string name);
    string getNext();
    void closeFile();
};

#endif // READER_H_INCLUDED
