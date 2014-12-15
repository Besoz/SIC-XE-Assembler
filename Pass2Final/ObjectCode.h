#ifndef OBJECTCODE_H_INCLUDED
#define OBJECTCODE_H_INCLUDED

#include<string>
#include"writer.h"
#include"Instruction.h"
#include <map>
#include <deque>

using namespace std;

class ObjectCode{
private :
    string line;
    int lineLength;
    string startingAddress;
    int MaxLength;
    writer writerObject;
    string lenghtOfProgram;
    string addressOfFirstInstruction;
    map<string,char>registerNumbers;
    deque<string> ModificationRecords;
    void createTextRecond(string record,int len,string address);
    void createModificationRecord(string address,int length);
    void createHeaderRecond(string record);
    void createEndRecord();
    void writeModificationRecords();
public :
    int hexStringToDecimel(string hexaString);
    string DecimalTohexString(int decimel,bool spaces);
    string toUpperCase(string key);
    void writeLine();
    ObjectCode(string length,string addressOfFirstInstr,string path);
    string addRecord(Instruction* instruct,string OperandLC,string OPCode,int format,bool relocatable);
    void printModificationRecords();
};

#endif // OBJECTCODE_H_INCLUDED
