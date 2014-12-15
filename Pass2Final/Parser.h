#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED
#include <string>
#include <iostream>
#include "reader.h"
#include "Instruction.h"
#include <vector>
#include <boost/regex.hpp>

using namespace std;
using namespace boost;
class Parser
{
    private:
            bool freeFormatAllowed,isPassTwo;
            reader file;
            void fixedFormat(Instruction *instruction,string line);
            void freeFormat(Instruction* instrcution,string line);
            bool checkIndexed(string operand);
            string getLabel(string line);
            string getOperand(string line);
            string getOperation(string line);
            vector<string>* splitter(string input);
            string toUpperCase(string key);
            bool checkLabel(string line);
            bool checkOperation(string line);
            bool checkOperand(string line,string operation);
            bool checkComma(string operand);

    public:
            Parser(string path,bool freeFormat,bool isPassTwo);
            Instruction* getNext();
            void closeFile();
            bool checkCommentLine(string line);
};

#endif // PARSER_H_INCLUDED
