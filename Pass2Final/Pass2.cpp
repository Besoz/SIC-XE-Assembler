#include <iostream>
#include "Pass2.h"
#include "Pass1.h"
#include "Instruction.h"
#include "OPTable.h"
#include "writer.h"
#include "Parser.h"
#include "ObjectCode.h"
#include <unordered_map>

using namespace std;

Instruction *inst;
writer* pass2writer;
OPTable *opTable;
Parser *parser;
Pass1*  pass1;
ObjectCode* objCW;
unordered_map<string,string>* LITTAB;
unordered_map<std::string , pair<string,int> >* symboleTable;

string addressingName2(int x);
string getFlags(Instruction* inst);


Pass2::Pass2(string path,string inputFile,bool freeFormat)
{
    inst = new Instruction();
    pass1 = new Pass1(path,"read.txt",freeFormat);
    pass1->run();
    parser = new Parser(path+"output.txt",freeFormat,true);
    pass2writer = pass1->getWriter();
    symboleTable = pass1->getSymTable();
    opTable = new OPTable(path+"opTable.txt");
    LITTAB = pass1->getLTTable();
}

void Pass2::run(string path)
{
    bool inLTORG = false;
    if(pass1->hasErrors())
        return;
    bool hasError = false;
    pass2writer ->writeToFile(">>>>   *****************************************************\n>>   S t a r t   o f    P a s s   I I\n");
    pass2writer ->writeToFile("lcCntr objCde    label opration operand\n\n");

    string startingAdd;

    while(inst!=NULL && inst->isCommented())
    {
        string s = inst->getLocation();
        s.append(" ");
        s.append("       ");
        s.append(inst->getInstruction());
        pass2writer ->writeToFile(s);
        pass2writer->writeToFile("\n");
        inst = parser->getNext();
        while(inst!=NULL && inst->isEmpty())
            inst = parser->getNext();
    }
    while(inst!=NULL && inst->isEmpty())
        inst = parser->getNext();
    objCW= new ObjectCode(pass1->getProgramLength(),inst -> getLocation(),path);
    if (inst!=NULL && (opTable -> toUpperCase(inst->getOperation()) == "START"))
    {
        string z=objCW->addRecord(inst,"","",0,false);

        string s = inst->getLocation();
        s.append(" ");;
        s.append("       ");
        s.append(inst->getInstruction());
        pass2writer ->writeToFile(s);
        pass2writer->writeToFile("\n");
        inst = parser->getNext();
    }

    while(inst!=NULL && opTable->toUpperCase(inst->getOperation()) != "END")
    {
        bool inLTORG = false;
        if (!inst -> isCommented())
        {

            if (opTable -> table.count(/*opCode*/opTable->toUpperCase(inst->getOperation())))
            {
                if(opTable->getFormat(opTable -> toUpperCase(inst->getOperation())) < 3)
                {
                    string s = inst->getLocation();
                    s.append(" ");
                    bool relocatable = false;
                    string z=objCW->addRecord(inst,pass1->getOperandLC(inst->getOperand()).first,opTable->getOPCode(inst -> getOperation()),opTable -> getFormat(inst -> getOperation()),relocatable);
                    if (z!="OverFlow Error")
                    {
                        s.append(z);
                    }
                    else
                {
                    pass2writer->writeToFile(" ****** overflow in Displacement field ");

                }
                    s.append(" ");
                    s.append(inst->getInstruction());
                    pass2writer ->writeToFile(s);
                    pass2writer->writeToFile("\n");

                }
                else if(opTable->toUpperCase(inst->getOperation()) != "RSUB")
                {

                    if (inst->getOperand()=="*" || symboleTable -> count(inst->getOperand()) || !pass1->getOperandLC(inst->getOperand()).second || (inst->getOperand()).at(0)=='=')
                    {
                        string s="                               ";
                        s.append(addressingName2(pass1->getOperandLC(inst->getOperand()).second));
                        s.append(getFlags(inst));
                        pass2writer->writeToFile(s);
                        s = inst->getLocation();
                        s.append(" ");
                        if(inst->getOperand()=="*")
                        {
                            string z =objCW->addRecord(inst,inst->getLocation(),opTable->getOPCode(inst -> getOperation()),opTable -> getFormat(inst -> getOperation()),1);


                            if (z!="OverFlow Error")
                            {
                                s.append(z);
                            }
                            else
                {
                    pass2writer->writeToFile(" ****** overflow in Displacement field ");

                }
                        }
                        else if((inst->getOperand()).at(0)=='=')
                        {
                            string z= objCW->addRecord(inst,(LITTAB->find(inst->getOperand()))->second,opTable->getOPCode(inst -> getOperation()),opTable -> getFormat(inst -> getOperation()),1);


                            if (z!="OverFlow Error")
                            {
                                s.append(z);
                            }
                            else
                {
                    pass2writer->writeToFile(" ****** overflow in Displacement field ");

                }
                        }
                        else
                        {
                            string z=objCW->addRecord(inst,pass1->getOperandLC(inst->getOperand()).first,opTable->getOPCode(inst -> getOperation()),opTable -> getFormat(inst -> getOperation()),pass1->getOperandLC(inst->getOperand()).second==1);



                            if (z!="OverFlow Error")
                            {
                                s.append(z);
                            }
                            else
                {
                    pass2writer->writeToFile(" ****** overflow in Displacement field ");

                }
                        }
                        s.append(" ");
                        s.append(inst->getInstruction());
                        pass2writer->writeToFile(s);
                        pass2writer->writeToFile("\n");
                    }
                    else
                    {
                        pass2writer->writeToFile(inst->getInstruction());
                        pass2writer->writeToFile(">>>>>>>>>>>> Error: Symbol not in symTable");
                        pass2writer->writeToFile("\n");

                    }
                }
                else
                {
                    string s = inst->getLocation();
                    s.append(" ");
                    string z=objCW->addRecord(inst,pass1->getOperandLC(inst->getOperand()).first,opTable->getOPCode(inst -> getOperation()),opTable -> getFormat(inst -> getOperation()),pass1->getOperandLC(inst->getOperand()).second==1);
                    cout << z<<endl;
                    if (z!="OverFlow Error")
                    {
                        s.append(z);
                    }
                    else
                    {
                        pass2writer->writeToFile(" ****** overflow in Displacement field ");

                    }
                    s.append(" ");
                    s.append(inst->getInstruction());
                    pass2writer->writeToFile(s);
                    pass2writer->writeToFile("\n");

                }
            }
            else if (opTable->toUpperCase(inst->getOperation()) == "WORD" ||
                     opTable->toUpperCase(inst->getOperation()) == "BYTE")
            {

                string s = inst->getLocation();
                s.append(" ");

                string z= objCW->addRecord(inst,pass1->getOperandLC(inst->getOperand()).first,"",0,pass1->getOperandLC(inst->getOperand()).second==1);


                if (z!="OverFlow Error")
                {
                    s.append(z);
                }
                else
                {
                    pass2writer->writeToFile("Error : over flow ");

                }
                s.append(" ");
                s.append(inst->getInstruction());
                pass2writer ->writeToFile(s);
                pass2writer->writeToFile("\n");
            }
            else if (opTable->toUpperCase(inst->getOperation()) == "RESW" ||
                     opTable->toUpperCase(inst->getOperation()) == "RESB" ||
                     opTable->toUpperCase(inst->getOperation()) == "EQU" ||
                     opTable->toUpperCase(inst->getOperation()) == "ORG")
            {
                if(opTable->toUpperCase(inst->getOperation()) == "RSEW" ||
                        opTable->toUpperCase(inst->getOperation()) == "RESB")
                {
                    objCW->writeLine();
                }
                string s = inst->getLocation();
                s.append(" ");
                s.append("       ");
                s.append(inst->getInstruction());
                pass2writer ->writeToFile(s);
                pass2writer->writeToFile("\n");
            }
            else if(inst->getOperation() == "LTORG")
            {
                while(((inst = parser->getNext())->getOperand()).at(0) == '=' && (inst->getOperation()).length() == 0)
                {
                    inst->setOperation(inst->getOperand());
                    string obCode = objCW->addRecord(inst,"","",0,false);
                    string s = inst->getLocation();
                    s.append(" ");
                    s.append(obCode);
                    s.append("       ");
                    s.append(inst->getOperation());
                    pass2writer->writeToFile(s);
                    pass2writer->writeToFile("\n");
                }
                inLTORG = true;
            }
            // write object
        }
        if(!inLTORG)
            inst = parser->getNext();
    }
    Instruction* newInstruction = new Instruction();
    while((newInstruction = parser->getNext()) != NULL  && (newInstruction->getOperand()).at(0) == '=')
    {
        newInstruction->setOperation(newInstruction->getOperand());
        string obCode = objCW->addRecord(newInstruction,"","",0,false);
        string str = newInstruction->getLocation();
        str.append(" ");
        str.append(obCode);
        str.append("       ");
        str.append(newInstruction->getOperation());
        pass2writer->writeToFile(str);
        pass2writer->writeToFile("\n");
    }
    objCW->addRecord(inst,inst->getLocation(),"",0,false);
    string s = inst->getLocation();
    s.append(" ");
    s.append("       ");
    s.append(inst->getInstruction());
    pass2writer ->writeToFile(s);
    pass2writer->writeToFile("\n");
}
string getFlags(Instruction* inst)
{

    string s;
    s.append("n=");
    if(inst->getFlagN())
    {
        s.append("1 ");
    }
    else if (inst->getFlagI())
    {
        s.append("0 ");
    }
    else
    {
        s.append("1 ");
    }
    s.append("i=");
    if(inst->getFlagI())
    {
        s.append("1 ");
    }
    else if(inst->getFlagN())
    {
        s.append("0 ");
    }
    else
    {
        s.append("1 ");
    }
    s.append("x=");
    if(inst->getFlagX())
    {
        s.append("1 ");
    }
    else
    {
        s.append("0 ");
    }
    s.append("b=");
    s.append("0 ");
    s.append("p=");
    if(inst->getFlagE() || (inst->getFlagI() && !inst->getFlagN()) || (!inst->getFlagI() && inst->getFlagN()))
    {
        s.append("0 ");
    }
    else
    {
        s.append("1 ");
    }
    s.append("e=");
    if(inst->getFlagE())
    {
        s.append("1 ");
    }
    else
    {
        s.append("0 ");
    }
    return s;
}
string addressingName2(int x)
{
    if (!x)
    {
        return "Abs ";
    }
    else
    {
        return "Rlc ";
    }
}
// write headre recored

// itialize firrst text recored

