#ifndef OPTABLE_H_INCLUDED
#define OPTABLE_H_INCLUDED
#include <fstream>
#include <string>
#include <map>
using namespace std;

class OPTable{
private :
public :
    map<string,pair<int,string> >table;
    OPTable(string path);
    int getFormat(string key);
    string getOPCode(string key);
    string toUpperCase(string key);
private :
    void initailizer(string path);
};

#endif // OPTABLE_H_INCLUDED
