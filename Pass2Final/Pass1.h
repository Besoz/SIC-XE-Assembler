#ifndef PASS1_H
#define PASS1_H
#include <unordered_map>
#include "writer.h"

using namespace std;

class Pass1
{
    public:
        Pass1(string path,string inputFile,bool freeFormat);
        void run();
        std::unordered_map<std::string , pair<string,int> >* getSymTable();
        writer* getWriter();
        string getProgramLength();
        pair<string,int> getOperandLC(string);
        bool hasErrors();
        std::unordered_map<std::string , string >* getLTTable();
    private:
        std::string startingAdd;
        std::string LocCounter;
        void advanceCounter(int);
        std::string getLocCounter();
       // bool checkOperand();
};


#endif // PASS1_H

