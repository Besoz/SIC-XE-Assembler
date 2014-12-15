#include <string>
#include <iostream>
#include "Parser.h"
#include "reader.h"
#include <ctype.h>
#include <string.h>
#include "Instruction.h"
#include <vector>
#include <boost/regex.hpp>

using namespace boost;
using namespace std;

Parser::Parser(string path,bool freeFormat, bool isPassTwo)
{
    this->freeFormatAllowed = freeFormat;
    file.openFile(path);
    this->isPassTwo = isPassTwo;
}

void Parser::closeFile()
{
    file.closeFile();
};

Instruction* Parser::getNext()
{
    if(file.myfile.eof())
    {
        closeFile();
        return NULL;
    }

    string line = file.getNext();

    Instruction* instruction = new Instruction();
    if(line.size()!=0)
    {
        // if isPassTwo = false then pass 1 else pass 2
        if(isPassTwo)
        {
            while(line.size()==0 || line.at(0) == '>' || line.at(0) == ' ')
            {
                line = file.getNext();
                if(file.myfile.eof())
                {
                    closeFile();
                    return NULL;
                }
            }
            instruction->setLocation(line.substr(0,6));
            line = line.substr(10);
            instruction->setInstruction(line);
        }
        else    instruction->setInstruction(line);

        if(freeFormatAllowed)  freeFormat(instruction,line);
        else fixedFormat(instruction,line);
    }

    return instruction;
};

/*
    label operation operand

    size whatever and start char is dot then comment
    size 1 -----> operation
    size 2 -----> label and operation , opeartion and operand
                LOOP    RSUB
                        JSUB     LOOP
    size 3
                     RMO        A, X
                     RMO        A ,X
            Label    RMO        A,X
    size 4
            Label    RMO        A, X
            Label    RMO        A ,X
                     RMO        A , X
    size 5
            Label    RMO        A , X


            loop 3ala al vector
            for each string belong to the vecto
            check if it contain '.'
            if so
            then the rest of strings are included in a comment field

*/
void Parser::freeFormat(Instruction* instrcution,string line)
{
    vector<string> separator = *splitter(line);

    // remove comment field
    for (int i=0;i<separator.size();i++)
    {
        if(separator[i][0]== '.')
        {
            int iterations = separator.size();
            for(int j = i ;j<iterations;j++)
            {
                separator.pop_back();
            }
            break;
        }
    }


    if(separator[0][0] == '.')  instrcution->setCommented(true);
    else
    {

        if(separator.size() == 1)
        {
            if(separator[0].size() > 15)   instrcution->setIllegalMnemonic(true);

            if(separator[0].size() > 66)  instrcution->setIllegalLength(true);

            if(separator[0][0]=='+')
            {
                instrcution->setFlagE(true);
                instrcution->setOperation(separator[0].substr(1,separator[0].size()-1));
            }
            else
            {
                instrcution->setOperation(separator[0]);
                instrcution->setFlagI(true);
                instrcution->setFlagN(true);
            }
        }
        else if(separator.size() == 2)
        {
                if(toUpperCase(separator[1]) == "RSUB" || toUpperCase(separator[1]) == "+RSUB" || toUpperCase(separator[1]) == "START")
                {
                    instrcution->setLabel(separator[0]);

                    if(separator[0].size() >8 )    instrcution->setIllegalLabel(true);
                    if(separator[1].size() >6 )    instrcution->setIllegalMnemonic(true);
                    if(separator[0].size()+ separator[1].size() >66 )   instrcution->setIllegalLength(true);

                    if(separator[1][0]=='+')
                    {
                        instrcution->setFlagE(true);
                        instrcution->setOperation(separator[1].substr(1,separator[1].size()-1));
                    }
                    else
                    {
                        instrcution->setOperation(separator[1]);
                        instrcution->setFlagI(true);
                        instrcution->setFlagN(true);
                    }
                }
                else
                {

                    if(separator[0].size() >6 ) instrcution->setIllegalMnemonic(true);
                    if(separator[1].size() >17 || checkComma(separator[1]) )    instrcution->setIllegalOperand(true);
                    if(separator[0].size()+ separator[1].size() >66 )   instrcution->setIllegalLength(true);

                    if(separator[0][0]=='+')
                    {
                        instrcution->setOperation(separator[0].substr(1,separator[0].size()-1));
                        instrcution->setFlagE(true);
                    }
                    else
                    {
                        instrcution->setOperation(separator[0]);
                        instrcution->setFlagI(true);
                        instrcution->setFlagN(true);
                    }
                    if(separator[1][0]=='#')
                    {
                        instrcution->setFlagI(true);
                        instrcution->setOperand(separator[1].substr(1,separator[1].size()-1));
                        if(separator[1].size()>5 ) instrcution->setIllegalOperand(true);

                    }
                    else if(separator[1][0]=='@')
                    {
                        instrcution->setFlagN(true);
                        instrcution->setOperand(separator[1].substr(1,separator[1].size()-1));

                    }
                    else if(checkIndexed(separator[1]))
                    {
                        instrcution->setFlagX(true);
                        instrcution->setOperand(separator[1].substr(0,separator[1].size()-2));

                    }
                    else
                    {
                        instrcution->setOperand(separator[1]);
                        instrcution->setFlagI(true);
                        instrcution->setFlagN(true);
                    }

                }
        }
        else if(separator.size() == 3)
        {
            if(separator[2][0]==',' || separator[1][separator[1].size()-1]==',' )
            {
                string operand = separator[1]+separator[2];

                if(separator[0].size() >6 ) instrcution->setIllegalMnemonic(true);
                if(operand.size() >17  || checkComma(operand))    instrcution->setIllegalOperand(true);
                if(separator[0].size()+ operand.size() >66 )   instrcution->setIllegalLength(true);

                if(separator[0][0]=='+')
                {
                    instrcution->setFlagE(true);
                    instrcution->setOperation(separator[0].substr(1,separator[0].size()-1));
                }
                else
                {
                    instrcution->setOperation(separator[0]);
                }

                if(operand[0]=='#')
                {
                    instrcution->setOperand(operand.substr(1,operand.size()-1));
                    instrcution->setFlagI(true);
                    if(operand.size()>5 ) instrcution->setIllegalOperand(true);
                }
                else if(operand[0]=='@')
                {
                    instrcution->setOperand(operand.substr(1,operand.size()-1));
                    instrcution->setFlagN(true);
                }
                else if(checkIndexed(operand))
                {
                    instrcution->setOperand(operand.substr(0,operand.size()-2));
                    instrcution->setFlagX(true);
                }
                else
                {
                    instrcution->setOperand(operand);
                    instrcution->setFlagI(true);
                    instrcution->setFlagN(true);
                }
            }
            else
            {
                if(separator[0].size() > 8) instrcution->setIllegalLabel(true);
                if(separator[1].size() > 6)    instrcution->setIllegalMnemonic(true);
                if(separator[2].size() > 17  || checkComma(separator[2]))    instrcution->setIllegalOperand(true);
                if(separator[0].size() +separator[1].size()+separator[2].size() > 66)    instrcution->setIllegalLength(true);

                instrcution->setLabel(separator[0]);
                if(separator[1][0]=='+')
                {
                    instrcution->setOperation(separator[1].substr(1,separator[1].size()-1));
                    instrcution->setFlagE(true);
                }
                else
                {
                    instrcution->setOperation(separator[1]);
                }

                if(separator[2][0]=='#')
                {
                    instrcution->setOperand(separator[2].substr(1,separator[2].size()-1));
                    instrcution->setFlagI(true);
                    if(separator[2].size()>5 ) instrcution->setIllegalOperand(true);
                }
                else if(separator[2][0]=='@')
                {
                    instrcution->setOperand(separator[2].substr(1,separator[2].size()-1));
                    instrcution->setFlagN(true);
                }
                else if(checkIndexed(separator[2]))
                {
                    instrcution->setOperand(separator[2].substr(0,separator[2].size()-2));
                    instrcution->setFlagX(true);
                }
                else
                {
                    instrcution->setOperand(separator[2]);
                    instrcution->setFlagI(true);
                    instrcution->setFlagN(true);
                }

            }
        }
        else if(separator.size() == 4)
        {
            // with label
            if(separator[3][0]==',' || (separator[2].size() >1  && separator[2][separator[2].size()-1]==',') )
            {
                string operand = separator[2]+separator[3];

                if(separator[0].size() > 8) instrcution->setIllegalLabel(true);
                if(separator[1].size() > 6)    instrcution->setIllegalMnemonic(true);
                if(operand.size() > 17 || checkComma(operand))    instrcution->setIllegalOperand(true);
                if(separator[0].size() +separator[1].size()+operand.size() > 66)    instrcution->setIllegalLength(true);

                instrcution->setLabel(separator[0]);

                if(separator[1][0]=='+')
                {
                    instrcution->setFlagE(true);
                    instrcution->setOperation(separator[1].substr(1,separator[1].size()-1));
                }
                else
                {
                    instrcution->setOperation(separator[1]);
                }
                if(operand[0]=='#')
                {
                    instrcution->setOperand(operand.substr(1,operand.size()-1));
                    instrcution->setFlagI(true);
                    if(operand.size()>5 ) instrcution->setIllegalOperand(true);
                }
                else if(operand[0]=='@')
                {
                    instrcution->setOperand(operand.substr(1,operand.size()-1));
                    instrcution->setFlagN(true);
                }
                else if(checkIndexed(operand))
                {
                    instrcution->setOperand(operand.substr(0,operand.size()-2));
                    instrcution->setFlagX(true);
                }
                else
                {
                    instrcution->setOperand(operand);
                    instrcution->setFlagI(true);
                    instrcution->setFlagN(true);
                }
            }
            //  without label
            else
            {
                string operand =  separator[1]+separator[2]+separator[3];
                if(separator[0].size() > 6)    instrcution->setIllegalMnemonic(true);
                if(operand.size() > 17 || checkComma(operand))    instrcution->setIllegalOperand(true);
                if(separator[0].size() +operand.size() > 66)    instrcution->setIllegalLength(true);


                if(separator[0][0]=='+')
                {
                    instrcution->setFlagE(true);
                    instrcution->setOperation(separator[0].substr(1,separator[0].size()-1));
                }
                else
                {
                    instrcution->setOperation(separator[0]);
                }

                if(operand[0]=='#')
                {
                    instrcution->setFlagI(true);
                    instrcution->setOperand(operand.substr(1,operand.size()-1));
                    if(operand.size()>5 ) instrcution->setIllegalOperand(true);
                }
                else if(operand[0]=='@')
                {
                    instrcution->setFlagN(true);
                    instrcution->setOperand(operand.substr(1,operand.size()-1));
                }
                else if(checkIndexed(operand))
                {
                    instrcution->setFlagX(true);
                    instrcution->setOperand(operand.substr(0,operand.size()-2));
                }
                else
                {
                    instrcution->setOperand(operand);
                    instrcution->setFlagI(true);
                    instrcution->setFlagN(true);
                }
            }
        }
        else if(separator.size() == 5)
        {
              string operand =  separator[2]+separator[3]+separator[4];

            if(separator[0].size() > 8) instrcution->setIllegalLabel(true);
            if(separator[1].size() > 6)    instrcution->setIllegalMnemonic(true);
            if(operand.size() > 17 || checkComma(operand))    instrcution->setIllegalOperand(true);
            if(separator[0].size() +separator[1].size()+operand.size() > 66)    instrcution->setIllegalLength(true);

            instrcution->setLabel(separator[0]);


            if(separator[1][0]=='+')
            {
                instrcution->setOperation(separator[1].substr(1,separator[1].size()-1));
                instrcution->setFlagE(true);
            }
            else
            {
                 instrcution->setOperation(separator[1]);
            }

            if(operand[0]=='#')
            {
                instrcution->setOperand(operand.substr(1,operand.size()-1));
                instrcution->setFlagI(true);
                if(operand.size()>5 ) instrcution->setIllegalOperand(true);
            }
            else if(operand[0]=='@')
            {
                instrcution->setOperand(operand.substr(1,operand.size()-1));
                instrcution->setFlagN(true);
            }
            else if(checkIndexed(operand))
            {
                instrcution->setOperand(operand.substr(0,operand.size()-2));
                instrcution->setFlagX(true);
            }
            else
            {
                instrcution->setOperand(operand);
                instrcution->setFlagI(true);
                instrcution->setFlagN(true);
            }
        }
    }
};

void Parser::fixedFormat(Instruction* instrcution,string line)
{
    if(line[0]=='.' || checkCommentLine(line))    instrcution->setCommented(true);
    else if(line.size()>9)
    {
        if(line.length() > 66)  instrcution->setIllegalLength(true);

        if( (line[8]!=' ' && line[8]!='+') || (line.size()>15 && line[15]!=' ') || (line.size()>16 && line[16]!= ' ') )
            instrcution->setUndefinedInstruction(true);

        string label = getLabel(line);
        if(checkLabel(line)) instrcution->setIllegalLabel(true);

        string operation = getOperation(line);
        if(checkOperation(line) )   instrcution->setIllegalMnemonic(true);

        string operand = getOperand(line);
        if(checkOperand(line,operation) || checkComma(operand)) instrcution->setIllegalOperand(true);

        instrcution->setOperation(operation);

        instrcution->setLabel(label);

        if(line[8]=='+')
        {
            instrcution->setFlagE(true);
        }

        if(operand[0]=='#')
        {
            instrcution->setFlagI(true);
            instrcution->setOperand(operand.substr(1,operand.size()-1));
            if(operand.size()>5 ) instrcution->setIllegalOperand(true);
        }
        else if(operand[0]=='@')
        {
            instrcution->setFlagN(true);
            instrcution->setOperand(operand.substr(1,operand.size()-1));
        }
        else if(checkIndexed(operand))
        {
            instrcution->setFlagX(true);
            instrcution->setOperand(operand.substr(0,operand.size()-2));
        }
        else
        {
            instrcution->setOperand(operand);
            instrcution->setFlagI(true);
            instrcution->setFlagN(true);
        }
    }
    else instrcution->setUndefinedInstruction(true);
};

bool Parser::checkIndexed(string operand)
{
    int length = 0;
    for(;length<operand.length();length++)
    {   if(operand.at(length)==',')    break;  }

    if(operand.length()==0 || length==operand.length() || length==operand.length()-1 || operand.length()<=length+1)
        return false;


    if( toupper(operand.at(length+1))=='X'
       &&
        (length+1==operand.length()-1||operand.at(length+2)==' '))
    {
        if(operand[1]==','
                            &&
                                (toupper(operand[0]) =='A'||toupper(operand[0]) =='B'
                                ||toupper(operand[0]) =='S' ||toupper(operand[0]) =='T'
                                ||toupper(operand[0]) =='X' ||toupper(operand[0]) =='F'
                                ||toupper(operand[0]) =='L'  ) )
        {
                return false ;
        }
        else return true;
    }

    return false;
};

bool Parser::checkComma(string operand)
{
    int length = 0;
    bool hasComma = false;
    for(;length<operand.length();length++)
    {   if(operand.at(length)==',')
        {
            hasComma = true;
            break;
        }
    }

    if((length==0 && operand.length()!=0) || length==operand.length()-1)
        return true;

    if(hasComma)
    {
        if(operand[1]==','
                            &&
                                (toupper(operand[0]) =='A'||toupper(operand[0]) =='B'
                                ||toupper(operand[0]) =='S' ||toupper(operand[0]) =='T'
                                ||toupper(operand[0]) =='X' ||toupper(operand[0]) =='F'
                                ||toupper(operand[0]) =='L'  ))
        {
            return false;
        }
        else
        {
            if( toupper(operand.at(length+1))=='X'
                             &&
                            (length+1==operand.length()-1||operand.at(length+2)==' '))
            {
                    return false;
            }
            else return true;
        }
    }
    return false;
};

string Parser::getLabel(string line)
{
    int length = 0;
    for(;length<8;length++)
    {   if(line.at(length)==' ')    break;  }
    if(length==0) return "";
    return line.substr(0,length);
};

bool Parser::checkLabel(string line)
{
    bool hasLabel = false;
    int length = 0;
    for(;length<8;length++)
    {   if(line.at(length)!=' ')
        {
            hasLabel= true;
            break;
        }
    }
    if(hasLabel)
    {
        if(!(toupper(line.at(0))>='A'&& toupper(line.at(0))<='Z')) return true;
        length = 0;
        for(;length<8;length++)
        {   if(line.at(length)==' ')    break;  }
        for(;length<8;length++)
        {   if(line.at(length)!=' ')    return true;  }
    }
    return false;
};

string Parser::getOperation(string line)
{
    int length = 9;
    for(;length<15 && length < line.length();length++)
    {   if(line.at(length)==' ')    break;  }
    if(length==9) return "";
    return line.substr(9,length-9);
};

bool Parser::checkOperation(string line)
{
    bool hasOperation = false;
    int length = 9;
    for(;length<15 && length < line.length();length++)
    {   if(line.at(length)!=' ')
        {
            hasOperation = true;
            break;
        }
    }
    if(hasOperation)
    {
        length = 9;
        for(;length<15 && length < line.length();length++)
        {   if(line.at(length)==' ')    break;  }
        for(;length<15 && length < line.length();length++)
        {   if(line.at(length)!=' ')    return true;  }
    }
    else return true;

    return false;
};


string Parser::getOperand(string line)
{
    int length = 17;
    for(;length<35 && length<line.length();length++)
    {   if(line.at(length)==' ')    break;  }

    if(length==17) return "";
    return line.substr(17,length-17);
};

bool Parser::checkOperand(string line,string operation)
{
    bool hasOperand = false;
    int length = 17;
    for(;length<35 && length<line.length();length++)
    {   if(line.at(length)!=' ')
        {
            hasOperand = true;
            break;
        }
    }

    if(hasOperand)
    {
        if(toUpperCase(operation) == "BYTE" || line.at(17)=='=')
        {
            int increment = (line.at(17)=='=')? 1 : 0;
            if(line.at(17+increment)!='X' && line.at(17+increment)!='C' )    return true;
            if(line.length() > 18+increment && line.at(18+increment)!='\'') return true;
            length = 19+increment;
            for(;length<35 && length < line.length();length++)
            {  if(line.at(length)=='\'') break;   }

            if(length==line.length() && line.at(length)!='\'')  return true;
            length++;
            for(;length<35 && length < line.length();length++)
            {
                 if(line.at(length)!=' ')  return true;
            }
            return false;
        }
        else
        {
            length = 17;
            for(;length<35 && length < line.length();length++)
            {   if(line.at(length)==' ')    break;  }
            for(;length<35 && length<line.length();length++)
            {   if(line.at(length)!=' ')    return true;  }
        }
    }
    return false;
};


vector<string>* Parser::splitter(string input)
{
    vector<string>* separator = new vector<string>();
    regex exp("([^ 	]+)");
    match_results<string::const_iterator> what;
    string::const_iterator start = input.begin();
    while ( regex_search(start, input.cend(), what, exp) )
    {
       separator->push_back(what[1]);
       start = what[0].second ;
    }
    return separator;
};

string Parser::toUpperCase(string key)
{
    string upperKey="";
    for(int i = 0 ; i < key.length(); i++)
    {   upperKey.push_back(toupper(key[i]));    }
    return upperKey;
};

bool Parser::checkCommentLine(string line)
{
    for(int i=0;i<line.size();i++)
    {
        if(line[i]!=' ')
        {
            if(line[i]=='.') return true;
            return false;
        }
    }
    return false;
};

