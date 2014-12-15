#include <iostream>
#include "Pass1.h"
#include "Instruction.h"
#include "OPTable.h"
#include "writer.h"
#include "Parser.h"
#include <sstream>
#include <unordered_map>
#include <stdlib.h>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "PostFix.h"

using namespace std;

Instruction *currentInst;
writer* myWriter;
OPTable *myOpTable;
Parser *myParser;
PostFix *postFix;
unordered_map<string , pair<string,int>> *symTable;
unordered_map<string , string> *LTTable;
int locCounterInt;
int getAddressType(string operand);
void writeSymTableToFile(writer* wP,  unordered_map<string , pair<string,int> >* symTable);
bool isRegister(string x,OPTable* optable);
bool isRegister2(char x);
bool checkOperand();
string intToString(int x);
string addressingName(int x);
bool hasError = false;

int toDecimal(string s)
{
    //std::string s
    char *a=new char[s.size()+1];
    a[s.size()]=0;
    memcpy(a,s.c_str(),s.size());
    int number;
    if (isxdigit(a[0]))
    {
        number = strtol (a,NULL,16);
    }
    return number;
}

Pass1::Pass1(string path,string inputFile,bool freeFormat)
{
    myWriter = new writer();
    myWriter ->createFile(path+"output.txt");
    symTable = new unordered_map<string , pair<string,int>>();
    // intiazlize optable
    currentInst = new Instruction();
    LTTable = new unordered_map<string , string>();
    myOpTable = new OPTable(path+"opTable.txt");
    // myWriter = new writer();
    myParser = new Parser(path+inputFile,freeFormat,false);
    // initialize postfix
    postFix = new PostFix();
}

bool symTableContain(string s)
{
    if(symTable->count(s) == 1 )
    {
        return 1;
    }
    return 0;
}
bool outOfRannge =false;
void Pass1::run()
{
    myWriter ->writeToFile(">>  Source Program statements with value of LC indicated\n\n");
    //  bool duplicateFlag = false,unrecognisedOp = false,hasOtherErrors;
    // get first line form parser
    currentInst = myParser->getNext();
    // if there is operand
    while(currentInst!=NULL && currentInst->isEmpty())
        currentInst = myParser->getNext();
    while(currentInst!=NULL && currentInst->isCommented())
    {
        string s = getLocCounter();
        s.append("    ");
        s.append(currentInst->getInstruction());
        myWriter ->writeToFile(s);
        currentInst = myParser->getNext();
        while(currentInst!=NULL && currentInst->isEmpty())
            currentInst = myParser->getNext();
    }
    if(currentInst!=NULL && myOpTable->toUpperCase(currentInst->getOperation()) == "START")
    {
        //  if(startAdd.length()<=4){
        // cout << atoi((currentInst->getOperand().c_str())) << endl;
        bool hasExtraCh=false,Illegal = false;
        if(currentInst->hasOperand() && !checkOperand())
        {
            Illegal = true;
            startingAdd = "00000";
            LocCounter = "00000";
            locCounterInt=0;
        }
        if(!Illegal && currentInst->hasOperand())
        {
            //  if(checkOperand())
            if(currentInst->getOperand().length()<=4)
            {
                startingAdd = currentInst->getOperand();
                LocCounter = startingAdd;
                locCounterInt=toDecimal(startingAdd);

                // cout << startingAdd << endl;
                //   if(getLocCounter()=="000000")
            }
            else
            {
                startingAdd = currentInst->getOperand().substr(0,4);
                LocCounter = startingAdd;
                locCounterInt=toDecimal(startingAdd);

                hasExtraCh = true;
            }
        }
        else
        {
            startingAdd = "00000";
            LocCounter = "00000";
            locCounterInt=0;

        }
        string s = getLocCounter();
        s.append("    ");
        s.append(currentInst->getInstruction());
        myWriter ->writeToFile(s);
        if(Illegal)
            myWriter ->writeToFile(" ****** illegal operand");
        //write to file
        //parse get next line
        if(hasExtraCh)
            myWriter ->writeToFile(" ****** extra characters at end of statement");
        currentInst = myParser->getNext();
        //   }
    }
    else
    {
        startingAdd = "00000";
        LocCounter = "00000";
        locCounterInt=0;

    }
    while(currentInst!=NULL && currentInst->isEmpty())
        currentInst = myParser->getNext();
    string literals[100];
    bool checkXorC[100];
    int pos = 0;
    while(currentInst!=NULL && myOpTable->toUpperCase(currentInst->getOperation()) != "END")
    {
        string s = getLocCounter();
        s.append("    ");
        s.append(currentInst->getInstruction());
        myWriter ->writeToFile(s);
        if(!currentInst->isCommented())
        {
            if(currentInst->hasLabel())
            {
                //search for symbol in table
                if (symTableContain(currentInst->getLabel()))
                {
                    // duplicateFlag = true;
                    hasError = true;
                    string s ="    ****Error Symbol:'";
                    string s1 =currentInst->getLabel();
                    string s2 ="' already defined ";
                    s = (s.append(s1)).append(s2);
                    myWriter ->writeToFile(s);
                }
                else if(myOpTable->toUpperCase(currentInst->getOperation()) != "EQU")
                {
                    //insert fel symbol table
                    //function to determine absolute or relocatable or invalid
                    //   if(atoi(currentInst->getOperand())==0 )
                    //  int temp = getAddressType(currentInst->getOperand());
                    //   if(temp==2){
                    //      myWriter ->writeToFile("   ****Illegal Operand ");
                    //  }else{
                    //  symTable->emplace(/*label */currentInst->getLabel() , make_pair(getLocCounter(),temp) );
                    symTable->emplace(/*label */currentInst->getLabel() , make_pair(getLocCounter(),getAddressType(currentInst->getLabel())));
                    // symTable->emplace(/*label */currentInst->getLabel() , getLocCounter() );
                    //  symTable[currentInst->getLabel()]=make_pair(getLocCounter(),temp);
                    // }
                }
            }
            //search for operation in optable
            bool noError=true;
            if(myOpTable -> table.count(/*opCode*/myOpTable->toUpperCase(currentInst->getOperation())))
            {
                //get format length from parser
                // 3 or 4
                //     cout << myOpTable->table.size() << endl;
                if(!currentInst->hasOperand() && myOpTable->toUpperCase(currentInst->getOperation())!="RSUB"
                        && myOpTable->getFormat(myOpTable->toUpperCase(currentInst->getOperation()))!=1)
                {
                    myWriter ->writeToFile(" ****** missing or misplaced operand field");
                    hasError = true;
                }
                if(currentInst->getOperand()[0]=='=')
                {
                    string op = myOpTable->toUpperCase(currentInst->getOperation());
                    if(currentInst->getOperand().length()>4 && myOpTable->toUpperCase(currentInst->getOperand())[1]=='C' && currentInst->getOperand()[2]==39 && currentInst->getOperand()[currentInst->getOperand().length()-1]==39)
                    {
                        int length = currentInst->getOperand().length()-4;
                        if(op.substr(0,2)=="LD" || op=="COMP" || op=="ADD" || op=="SUB" || op=="MUL" || op=="DIV" || op=="LPS")
                        {
                            if((op=="LDCH" && length!=1) || (op!="LDCH" && length%3!=0))
                                myWriter ->writeToFile(" ****** invalid literal length ");
                            else
                            {
                                checkXorC[pos] = false;
                                literals[pos++] = currentInst->getOperand();
                            }
                        }
                        else if(op=="TD" || op=="WD" || op=="RD")
                        {
                            if(length!=1)
                                myWriter ->writeToFile(" ****** invalid literal length ");
                            else
                            {
                                checkXorC[pos] = false;
                                literals[pos++] = currentInst->getOperand();
                            }
                        }
                        else
                        {
                            myWriter ->writeToFile(" ****** wrong use of a literal ");
                        }
                    }
                    else if(currentInst->getOperand().length()>4 && myOpTable->toUpperCase(currentInst->getOperand())[1]=='X' && currentInst->getOperand()[2]==39 && currentInst->getOperand()[currentInst->getOperand().length()-1]==39)
                    {
                        int length = currentInst->getOperand().length()-4;
                        if(op.substr(0,2)=="LD" || op=="COMP" || op=="ADD" || op=="SUB" || op=="MUL" || op=="DIV" || op=="LPS")
                        {
                            if((op=="LDCH" && length!=2) || (op!="LDCH" && length%6!=0))
                                myWriter ->writeToFile(" ****** invalid literal length ");
                            else
                            {
                                checkXorC[pos] = true;
                                literals[pos++] = currentInst->getOperand();
                            }
                        }
                        else if(op=="TD" || op=="WD" || op=="RD")
                        {
                            if(length!=2)
                                myWriter ->writeToFile(" ****** invalid literal length ");
                            else
                            {
                                checkXorC[pos] = true;
                                literals[pos++] = currentInst->getOperand();
                            }
                        }
                        else
                        {
                            myWriter ->writeToFile(" ****** wrong use of a literal ");
                        }
                    }
                    else
                    {
                        myWriter ->writeToFile(" ****** wrong use of a literal ");
                    }
                }
                if(myOpTable->getFormat(myOpTable->toUpperCase(currentInst->getOperation()))==1)
                    advanceCounter(1);
                else if(myOpTable->getFormat(myOpTable->toUpperCase(currentInst->getOperation()))==2)
                {

                    string bb=currentInst->getOperation();
                    bb=myOpTable->toUpperCase(bb);
                    if (bb.compare("CLEAR")==0 || bb.compare("TIXR")==0)
                    {
                        // one register only
                        if (currentInst->getOperand().size()!= 1)
                        {
                            myWriter -> writeToFile(" ****** Error: undefined operand");
                        }
                    }
                    else if (currentInst->getOperand().size()!= 3)
                    {
                        myWriter -> writeToFile("  ****** missing comma in operand field ");
                    }
                    if(!isRegister(currentInst->getOperand(),myOpTable) )
                    {
                        myWriter -> writeToFile(" ****** illegal address for a register ");
                    }
                    advanceCounter(2);
                }
                else if(myOpTable->getFormat(myOpTable->toUpperCase(currentInst->getOperation()))==3 && !currentInst->getFlagE())
                    advanceCounter(3);
                else if(myOpTable->getFormat(myOpTable->toUpperCase(currentInst->getOperation()))==3 && currentInst->getFlagE())
                    advanceCounter(4);

            }
            else if(myOpTable->toUpperCase(currentInst->getOperation()) == "LTORG")
            {
                for(int i=0; i<pos; i++)
                {
                    int tmp = literals[i].length()-4;
                    LTTable->emplace(literals[i],getLocCounter());
                    string s = getLocCounter();
                    s.append("                     ");
                    s.append(literals[i]);
                    myWriter ->writeToFile(s);
                    if(checkXorC[i])
                        advanceCounter(tmp/2);
                    else
                        advanceCounter(tmp);
                }
                pos = 0;
            }
            else if(myOpTable->toUpperCase(currentInst->getOperation()) == "WORD")
            {
                if(!currentInst->hasOperand())
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** missing or misplaced operand field");
                }
                advanceCounter(3);
            }
            else if(myOpTable->toUpperCase(currentInst->getOperation()) == "RESW")
            {
                if(!currentInst->hasOperand() )
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** missing or misplaced operand field");
                }
                else if( atoi((currentInst->getOperand().c_str()))<=0)
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** illegal operand");
                }
                else if(currentInst->getOperand().length()>4)
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** extra characters at end of statement");
                }
                advanceCounter(3 * atoi((currentInst->getOperand().c_str()))); //* number of words (getoperand)
            }
            else if(myOpTable->toUpperCase(currentInst->getOperation()) == "RESB")
            {
                if(!currentInst->hasOperand() )
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** missing or misplaced operand field");
                }
                else if( atoi((currentInst->getOperand().c_str()))<=0)
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** illegal operand");
                }
                else if(currentInst->getOperand().length()>4)
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** extra characters at end of statement");
                }
                advanceCounter(atoi((currentInst->getOperand().c_str())));//* number of bytes
            }
            else if(myOpTable->toUpperCase(currentInst->getOperation()) == "BYTE")
            {
                //shoof el length beta3oh
                if(!currentInst->hasOperand())
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** missing or misplaced operand field");
                }
                if(currentInst->getOperand().length()>3 && myOpTable->toUpperCase(currentInst->getOperand())[0]=='C' && currentInst->getOperand()[1]==39 && currentInst->getOperand()[currentInst->getOperand().length()-1]==39)
                {
                    advanceCounter(currentInst->getOperand().length()-3);
                }
                else if(currentInst->getOperand().length()>3 && myOpTable->toUpperCase(currentInst->getOperand())[0]=='X' && currentInst->getOperand()[1]==39 && currentInst->getOperand()[currentInst->getOperand().length()-1]==39)
                {
                    if((currentInst->getOperand().length()-3)%2)
                    {
                        hasError = true;
                        myWriter ->writeToFile(" ****** odd length for hex string ");
                    }
                    advanceCounter((currentInst->getOperand().length()-3)/2);
                }
                else
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** illegal operand ");
                }
            }
            else if(myOpTable->toUpperCase(currentInst->getOperation()) == "EQU")
            {
                if(!currentInst->hasLabel())
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** this statement requires a label ");
                }
                else
                {
                    //  if(symTable->count(myOpTable->toUpperCase(currentInst->getOperand())) == 0){
                    //      myWriter ->writeToFile(" ****** undefined symbol in operand ");
                    //  }
                    // else{
                    pair<string,int> temp = getOperandLC(currentInst->getOperand());
                    //    cout << temp.first << endl;
                    if(temp.second==2)
                    {
                        hasError = true;
                        myWriter ->writeToFile(" ****** illegal operand ");
                    }
                    else if(temp.second==4)
                    {
                        hasError = true;
                        myWriter ->writeToFile(" ****** extra characters at end of statement ");
                    }
                    else
                    {
                        symTable->emplace(/*label */currentInst->getLabel() , make_pair(temp.first,temp.second));
                    }
                    //  }
                }
            }
            else if(myOpTable->toUpperCase(currentInst->getOperation()) == "ORG")
            {
                if(currentInst->hasLabel())
                {
                    hasError = true;
                    myWriter ->writeToFile(" ****** this statement can not have a label ");
                }
                else
                {
                    pair<string,int> temp = getOperandLC(currentInst->getOperand());
                    //   cout << temp.first << endl;
                    if(temp.second==2)
                    {
                        hasError = true;
                        myWriter ->writeToFile(" ****** illegal operand ");
                    }
                    else if(temp.second==4)
                    {
                        hasError = true;
                        myWriter ->writeToFile(" ****** extra characters at end of statement ");
                    }
                    else
                    {
                        LocCounter = temp.first;
                    }
                    // handalet el location counter !!
                }
            }
            else
            {
                // unrecognisedOp = true;
                hasError = true;
                noError=false;
                myWriter ->writeToFile(" ****** unrecognized operation code ");
            }
            if (currentInst->isUndefinedInstruction()&& noError)
            {
                hasError = true;
                myWriter->writeToFile(" ****** Error: unrecognized instruction ");
            }
            if (currentInst ->hasIllegalMnemonic()&& noError)
            {
                hasError = true;
                myWriter->writeToFile(" ****** unrecognized mnemonic ");
            }
            else if(currentInst ->hasIllegalLength() && noError)
            {
                hasError = true;
                myWriter->writeToFile(" ****** Error: Illegal intsruction length ");
            }
            if (currentInst ->hasIllegalLabel()&& noError)
            {
                hasError = true;
                myWriter->writeToFile(" ****** Error: label longer than 9 chars ");
            }

            if (currentInst ->hasIllegalOperand()&& noError)
            {
                hasError = true;
                myWriter->writeToFile(" ****** Error: operand missing or has extra characters ");
            }

            //write el line
            //   string s = getLocCounter();
            //   s.append("    ");
            //   s.append(currentInst->getInstruction());
            //   myWriter ->writeToFile(s);
            //   if(duplicateFlag){
            //   duplicateFlag = false;

            //write "    ****Error Symbol:'kaza' already defined "
            //   }else if(unrecognisedOp){
            //   unrecognisedOp = false;

            // write " ****** unrecognized operation code "
            //  }

            //read next line form parser
            //  currentInst = myParser->getNext();
        }
        currentInst = myParser->getNext();
        while(currentInst!=NULL && currentInst->isEmpty())
            currentInst = myParser->getNext();
    }
    // cout << currentInst->getInstruction() << endl;
    if(currentInst!=NULL)
    {
        string s = getLocCounter();
        s.append("    ");
        s.append(currentInst->getInstruction());
        myWriter ->writeToFile(s);
    }
    else
    {
        hasError = true;
        myWriter ->writeToFile(" ****** End statement does not exist ");
    }
    for(int i=0; i<pos; i++)
    {
        int tmp = literals[i].length()-4;
        LTTable->emplace(literals[i],getLocCounter());
        string s = getLocCounter();
        s.append("                     ");
        s.append(literals[i]);
        myWriter ->writeToFile(s);
        if(checkXorC[i])
            advanceCounter(tmp/2);
        else
            advanceCounter(tmp);
    }

    myWriter ->writeToFile(">>   *****************************************************");
    if(hasError || outOfRannge)
        myWriter ->writeToFile(">>    i n c o m p l e t e    a s s e m b l y\n");
    else
        writeSymTableToFile(myWriter,symTable);

    myWriter ->writeToFile(">>    e n d    o f   p a s s   1 \n");

}

void writeSymTableToFile(writer* wP,  unordered_map<string , pair<string,int> >* symTable)
{

    if(symTable->size()==0)
        return;

    int max=0;
    wP -> writeToFile("\n");
    wP -> writeToFile(">>    s y m b o l     t a b l e   (values in hexa)");
    wP -> writeToFile("\n");
    string s;
    vector<string> vec1;
    vector<string> vec2;
    vector<int> vec3;
    for ( auto it = symTable->cbegin(); it != symTable->cend(); ++it )
    {
        vec1.push_back(it->first);
        vec2.push_back(it->second.first);
        vec3.push_back(it->second.second);
        if(max < vec1.at(vec1.size()-1).size())
        {
            max=vec1.at(vec1.size()-1).size();
        }
    }
    string k="       Symbol";
    string spcs(max-1,' ');
    k.append(spcs);
    k.append("Hexa Value");
    k.append("     ");
    k.append("Addressing type");
    wP -> writeToFile(k);
    string dashz="       ";
    string dashz2(k.size()-7,'-');
    dashz.append(dashz2);
    wP -> writeToFile(dashz);
    for (int i= vec1.size()-1 ; i >= 0 ; i --)
    {
        string spcs((max-vec1.at(i).size())+5,' ');
        string k="       ";
        k.append(vec1.at(i));
        k.append(spcs);
        k.append(vec2.at(i));
        k.append("         ");
        k.append(addressingName(vec3.at(i)));
        wP -> writeToFile(k);
    }
    wP -> writeToFile(">>   *****************************************************");
    wP -> writeToFile("\n");
    return ;
}
string Pass1::getLocCounter()
{
    if(LocCounter.length()==6)
        return LocCounter;
    string s ="0";
    for (int i=LocCounter.length()+1; i<6; i++)
        s.append("0");
    s.append(LocCounter);
    s = myOpTable->toUpperCase(s);
    return s;
}

void Pass1::advanceCounter(int instructionLnth)
{
    locCounterInt+=instructionLnth;
    std::istringstream buffer(LocCounter);
    unsigned int value;
    buffer >> std::hex >> value;
    std::stringstream stream;
    stream << hex << value+instructionLnth;
    LocCounter =( stream.str() );
    LocCounter = myOpTable->toUpperCase(LocCounter);
    if (locCounterInt >= 12343)
    {
        myWriter->writeToFile(" ****** address out of range");
        outOfRannge=true;
    }
}
bool isRegister(string x,OPTable* optable)
{
    x=optable->toUpperCase(x);
    if (x.size()==3)
    {
        if (isRegister2(x[0]) && isRegister2(x[2]))
        {
            return 1;
        }
    }
    else if (x.size()==1 && isRegister2(x[0]))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
bool isRegister2(char x)
{
    if (x=='A' ||x=='X'||x=='L'||x=='B'||x=='S'||x=='T'||x=='F' )
    {
        return 1;
    }
    return 0;
}
bool checkOperand()
{
    // if(atoi((currentInst->getOperand().c_str()))==0 && (currentInst->getOperand()=="0" || currentInst->getOperand()=="00"
    //        || currentInst->getOperand()=="000" || currentInst->getOperand()=="0000" || currentInst->getOperand()=="00000" || currentInst->getOperand()=="000000") )
    //         return true;
    for(int i=0; i<currentInst->getOperand().length(); i++)
    {
        //    if(!((currentInst->getOperand()[i]<='e'
        //      && currentInst->getOperand()[i]>='a')|| (currentInst->getOperand()[i]<='E'
        //      && currentInst->getOperand()[i]>='A')))
        //      return false;
        if(!((currentInst->getOperand()[i]>='0' && currentInst->getOperand()[i]<='9') || (currentInst->getOperand()[i]<='e'
                && currentInst->getOperand()[i]>='a') || (currentInst->getOperand()[i]<='E'
                        && currentInst->getOperand()[i]>='A')))
            return false;
    }
    return true;
}
string intToString(int x)
{
    stringstream ss;
    ss << x;
    string s(ss.str());
    return s;
}
int getAddressType(string operand)
{
    for(int i=0; i<operand.length(); i++)
    {
        if(operand.at(i)=='*' || operand.at(i)=='/' || operand.at(i)==')' )
            return 2;
        if(operand.at(i)=='+' || operand.at(i)=='-' || operand.at(i)=='(' )
            return 1;
        // roo7 le matrix
    }
    if((atoi(operand.c_str())!=0 && operand.length()<=4) || operand=="0" || operand=="00" || operand=="000"
            || operand=="0000")
        return 0;
    return 1;
}

pair<string,int> Pass1::getOperandLC(string operand)
{
    if(operand=="*")
        return make_pair(getLocCounter(),1);
    //  cout << operand << endl;
    for(int i=0; i<operand.length(); i++)
    {
         if(operand.at(i)=='/' || operand.at(i)==')' )
             return make_pair("",2);
        if(operand.at(i)=='+' || operand.at(i)=='-' || operand.at(i)=='(' )
        {
            postFix->infixToPostfix(operand);
            return postFix->evaluatePostfix(symTable,getLocCounter());
        }
        // roo7 le matrix
    }
    if((atoi(operand.c_str())>0 && operand.length()<=4) || operand=="0" || operand=="00" || operand=="000"
            || operand=="0000")
        return make_pair(operand,0);
    else if(atoi(operand.c_str()) !=0 && operand.length()>4)
    {
        return make_pair("",4);
    }
    if(symTable->count(myOpTable->toUpperCase(operand)) != 0)
    {
        //   cout << symTable->at(myOpTable->toUpperCase(operand)).first << endl;
        return make_pair(symTable->at(myOpTable->toUpperCase(operand)).first,1);
    }
    return make_pair("",2);
}

string addressingName(int x)
{
    if (!x)
    {
        return "  Absolute";
    }
    else
    {
        return "Relocatable";
    }
}

unordered_map<std::string , pair<string,int> >* Pass1::getSymTable()
{
    return symTable;
}

writer* Pass1::getWriter()
{
    return myWriter;
}

unordered_map<string, string> *Pass1::getLTTable()
{
    return LTTable;
}

bool Pass1::hasErrors()
{
    return hasError;
}
string Pass1::getProgramLength()
{
    string length;
    std::istringstream buffer(LocCounter);
    unsigned int value1;
    buffer >> std::hex >> value1;
    std::istringstream buffer1(startingAdd);
    unsigned int value2;
    buffer1 >> std::hex >> value2;
    //  cout << value1 <<" " << value2<<endl;
    std::stringstream stream;
    //  cout << value1-value2 << endl;
    stream << hex << value1-value2;
    length =( stream.str() );
    if(length.length()==6)
        return length;
    string s = "0";
    for (int i=length.length()+1; i<6; i++)
        s.append("0");
    s.append(length);
    s = myOpTable->toUpperCase(s);
    return s;
}
