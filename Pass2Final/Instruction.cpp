#include "Instruction.h"
#include <iostream>

using namespace std;

//constructor
    Instruction::Instruction()
    {
        this->label="";
        this->operation="";
        this->operand="";
        this->instruction="";
        this->absolute=false;
        this->comment=false;
        this->flagN=false;
        this->flagI=false;
        this->flagX=false;
        this->flagE=false;
        this->illegalLabel=false;
        this->illegalMnemonic=false;
        this->illegalOperand=false;
        this->illegalLength=false;
        this->undefinedInstruction=false;
        this->location="";

    }

//getters
    bool Instruction::isUndefinedInstruction()
    {
        return undefinedInstruction;
    };
    bool Instruction::isEmpty()
    {
        return (instruction.size()==0);
    };
    bool Instruction::hasIllegalOperand()
    {
      return illegalOperand;
    };
    bool Instruction::hasIllegalLabel(){
        return illegalLabel;
    };
    bool Instruction::hasIllegalMnemonic(){
        return illegalMnemonic;
    };
    bool Instruction::hasIllegalLength(){
        return illegalLength;
    };
    string Instruction::getInstruction()
    {
      return instruction;
    };
    string Instruction::getLabel()
    {
        return label;
    };
    string Instruction::getOperation(){
        return operation;
    };
    string Instruction::getOperand(){
        return operand;
    };
    string Instruction::getLocation(){
        return location;
    };
    bool Instruction::hasLabel(){
        return (label!="");
    };
    bool Instruction::hasOperand()
    {
        return (operand!="");
    };
    bool Instruction::isAbsolute()
    {
        return absolute;
    };
    bool Instruction::isCommented()
    {
        return comment;
    };
    bool Instruction::getFlagN()
    {
        return flagN;
    };
    bool Instruction::getFlagI()
    {
        return flagI;
    };
    bool Instruction::getFlagX()
    {
        return flagX;
    };
    bool Instruction::getFlagE()
    {
        return flagE;
    };

    //setters
    void Instruction::setLocation(string location)
    {
      this->location = location;
    };
    void Instruction::setUndefinedInstruction(bool undefinedInstruction)
    {
        this->undefinedInstruction = undefinedInstruction;
    };
    void Instruction::setIllegalOperand(bool illegalOperand)
    {
      this->illegalOperand = illegalOperand;
    };
    void Instruction::setIllegalLabel(bool illegalLabel)
    {
        this->illegalLabel = illegalLabel;
    };
    void Instruction::setIllegalMnemonic(bool illegalMnemonic)
    {
        this->illegalMnemonic = illegalMnemonic;
    };
    void Instruction::setIllegalLength(bool illegalLength)
    {
        this->illegalLength = illegalLength;
    };

    void Instruction::setInstruction(string instruction)
    {
        this->instruction = instruction;
    };

    void Instruction::setLabel(string label)
    {
        this->label = label;
    };
    void Instruction::setOperation(string operation)
    {
        this->operation = operation;
    };
    void Instruction::setOperand(string operand)
    {
        this->operand = operand;
    };
    void Instruction::setAbsolute(bool absolute)
    {
        this->absolute = absolute;
    };
    void Instruction::setCommented(bool comment)
    {
        this->comment = comment;
    };
    void Instruction::setFlagN(bool flagN)
    {
        this->flagN = flagN;
    };
    void Instruction::setFlagI(bool flagI)
    {
        this->flagI = flagI;
    };
    void Instruction::setFlagX(bool flagX)
    {
        this->flagX = flagX;
    };
    void Instruction::setFlagE(bool flagE)
    {
        this->flagE = flagE;
    };


