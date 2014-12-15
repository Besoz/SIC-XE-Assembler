#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED
#include <fstream>
#include <string>

using namespace std;

class Instruction
{
private :

    string label,operation,operand,instruction,location;
    bool absolute,comment,flagN,flagI,flagX,flagE,undefinedInstruction;
    bool illegalLabel,illegalMnemonic,illegalOperand,illegalLength;
public :
    //constructor
    Instruction();
    //getters
    string getLocation();
    string getInstruction();
    string getLabel();
    string getOperation();
    string getOperand();
    bool isUndefinedInstruction();
    bool isEmpty();
    bool hasLabel();
    bool hasOperand();
    bool hasIllegalLabel();
    bool hasIllegalMnemonic();
    bool hasIllegalLength();
    bool hasIllegalOperand();
    bool isAbsolute();
    bool isCommented();
    bool getFlagN();
    bool getFlagI();
    bool getFlagX();
    bool getFlagE();
    //setters
    void setLocation(string location);
    void setUndefinedInstruction(bool undefinedInstruction);
    void setIllegalLabel(bool illegalLabel);
    void setIllegalMnemonic(bool illegalMnemonic);
    void setIllegalOperand(bool illegalOperand);
    void setIllegalLength(bool illegalLength);
    void setInstruction(string instruction);
    void setLabel(string label);
    void setOperation(string operation);
    void setOperand(string operand);
    void setAbsolute(bool absolute);
    void setCommented(bool comment);
    void setFlagN(bool flagN);
    void setFlagI(bool flagI);
    void setFlagX(bool flagX);
    void setFlagE(bool flagE);
};
#endif // INSTRUCTION_H_INCLUDED
