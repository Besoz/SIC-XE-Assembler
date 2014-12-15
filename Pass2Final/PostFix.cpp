/*

------------------------------------------------------------------------------------------------------
All Cases:
----------
L1  RESW 1 (relative)
L2  RESW 2 (relative)

L3  EQU L1-L2 (absolute)
L4 EQU L1 - L2 (absolute)

L5 EQU  L3 - L2 (Invalid)  a - (b+start) = k - start , (k = a-b)
L6 EQU L3 + L2 (relative)
L7 EQU L2 - L3 (relative)
L8 EQU L2 + L3 (relative)

L9 EQU L3 + L4 (absolute)
L10 EQU L3 - L4 (absolute)
L11 EQU L1 - L2 (absolute)
L12 EQU L1 + L2 (invalid)
-----------------------------------------------------------------------------------------------------------
*/
#include <string>
#include <iostream>
#include <boost/regex.hpp>
#include <stack>
#include<unordered_map>
#include<string>
#include "PostFix.h"
#include <utility>

using namespace boost;
using namespace std;

//http://www.regexr.com/
vector<string> PostFix::generateInfix(string operand)
{
    //check bracites
    if(checkBrackets(operand))
    {
        //invalid expression
        this->type = 2 ;
        this->address = "000000" ;
        return (vector<string>());
    }
    //checking expression
    vector<string>* separator = new vector<string>();

    regex exp0("([*]+[^)+-]+|[^(+-]+[*])");
    //NOT ( *- -* +* -*)
    if(regex_search(operand,exp0))
    {
        //invalid expression
        this->type = 2 ;
        this->address = "000000" ;
        return (vector<string>());
    }

    regex exp1("([^a-zA-Z0-9()*+-]+)");
    if(regex_search(operand,exp1))
    {
        //invalid expression
        this->type = 2 ;
        this->address = "000000" ;
        return (vector<string>());
    }
    else
    {
        regex exp2("[^-+*()]+|[()*+-]");
        match_results<string::const_iterator> what;
        string::const_iterator start = operand.begin();
        while ( regex_search(start, operand.cend(), what, exp2) )
        {
           separator->push_back(what[0]);
           start = what[0].second ;
        }
        return *separator;
    }

};

bool PostFix::checkBrackets(string operand)
{
    int check = 0;
    for(int i = 0 ; i < operand.length() ; i++)
    {
        if(operand.at(i)=='(')  check++;
        else if(operand.at(i)==')') check--;
    }
    return (check!=0);
};

/*
Infix to postfix
----------------
Any Variable >> add to postfix expression
Left Parenthesis >> push onto the stack and remove only when the right parenthesis is found
Right Parenthesis >> remove symbol from stack to postfix expression till incounter a left parenthesis then discard it.
Operator >>   it's precedence is surely equal to that in the top of the stack since we only have + or -
              then pop the element on the top of the stack to the post fix expression till the stack is empty
              then push the operator in.
End of Expression >> Pop all the stack to the post fix expression
----------------------------------
*/
void PostFix::infixToPostfix(string operand)
{
    this->postFix = vector<string>();
    vector<string> infix = generateInfix(operand);
    // may be valid expression
    if(this->type!=2)
    {
        stack<string> stack;
        for(int i = 0 ; i < infix.size() ; i++)
        {
            if(infix[i] == "+" || infix[i] == "-" )
            {
                while(!stack.empty() && stack.top() != "(")
                {
                      this->postFix.push_back(stack.top());
                      stack.pop();
                }
                stack.push(infix[i]);
            }
            else  if(infix[i] == "(" )  stack.push(infix[i]);
            else if(infix[i] == ")" )
            {
                while( stack.top() != "(")
                {
                    this->postFix.push_back(stack.top());
                    stack.pop();
                }
                stack.pop();
            }
            else    this->postFix.push_back(infix[i]);
        }

        while(!stack.empty())
        {
            this->postFix.push_back(stack.top());
            stack.pop();
        }
    }
};

/*
Evaluating Postfix
------------------
1- Scan postfix expression from left to right
2- Let the 1st element of the postfix expression be X ,Push X to the stack
3- Let the second element be Y, Push Y to the stack.
4- The third element must be an operator. let it be ()
5- Evaluate Z = X () Y
6- Push Z to the stack
7- Continue till the end of the post fix expression
*/
// [0]address evaluation , [1]type of address
pair<string,int> PostFix::evaluatePostfix(unordered_map<string , pair<string,int> > *symTable , string locationCounter)
{
    // if not type = 2 then eval else return type = 2 and address = 000000
    if(this->type!=2)
    {
        stack<string> addressStack;
        stack<int> typeStack;
        int secondOperand,firstOperand,secondType,firstType,evaluation,resultedType;
        for ( int i = 0 ; i < this->postFix.size() ; i++)
        {
            if(this->postFix[i] == "+" || this->postFix[i] == "-")
            {
                secondOperand =   hexStringToDecimel(addressStack.top());
                addressStack.pop();
                firstOperand =   hexStringToDecimel(addressStack.top());
                addressStack.pop();
                secondType = typeStack.top();
                typeStack.pop();
                firstType = typeStack.top();
                typeStack.pop();
                evaluation = (this->postFix[i] == "+")?(firstOperand+secondOperand):(firstOperand-secondOperand);
                resultedType = (this->postFix[i] == "+")?computeType(secondType,firstType,'+'):computeType(secondType,firstType,'-');

                if(resultedType!=2)
                {
                    addressStack.push(DecimalTohexString(evaluation));
                    typeStack.push(resultedType);
                }
                else    return make_pair("000000",2);
            }
            else
            {
                // check if location counter or label or not
                if(postFix[i]=="*")
                {
                    addressStack.push(locationCounter);
                    typeStack.push(1);
                }
                else if( postFix[i].at(0)>'9' )
                {
                    addressStack.push( (symTable->find(this->postFix[i])->second).first );
                    typeStack.push( (symTable->find(this->postFix[i])->second).second );
                }
                else
                {
                    addressStack.push(this->postFix[i]);
                    typeStack.push(0);
                }
            }
        }
        pair<string,int> p = make_pair(addressStack.top(),typeStack.top());
        typeStack.pop();
        // check if out of bound then invalid
        if(hexStringToDecimel(addressStack.top()) < 0 || hexStringToDecimel(addressStack.top()) > 1048576)
        {
            p.first = "000000";
            p.second= 2;
        }
        addressStack.pop();
        return p ;
    }
    else    return make_pair("000000",2);

};

/*
-----------------------------------------------
(A)bsolute(0) or (R)elative(1) or (I)nvalid(2)
-----------------------------------------------
        (+)          (-)
-----------------------------------------------
0 0      A            A
0 1      R            I
0 2      I            I
1 0      R            R
1 1      I            A
1 2      I            I
2 0      I            I
2 1      I            I
2 2      I            I
-----------------------------------------------
*/
int PostFix::computeType(int secondType,int firstType, char operation)
{
    if(operation=='+')
    {
        if( (firstType== 0 && secondType == 1) || (firstType== 1 && secondType == 0)) return 1;
        else if(firstType== 0 && secondType == 0) return 0;
        return 2;
    }
    else
    {
        if( (firstType== 0 && secondType == 0) || (firstType== 1 && secondType == 1)) return 0;
        else if(firstType== 1 && secondType == 0) return 1;
        return 2;
    }
}

int PostFix::hexStringToDecimel(string hexaString)
{
    int x;
    stringstream stream;
    stream <<hex<< hexaString;
    stream >> x;
    return x;
};

string PostFix::DecimalTohexString(int decimel)
{
   stringstream stream;
   stream<<hex<<decimel;
   string res (stream.str());
   res = toUpperCase(res);
   if(res.size()<6)
   {
       switch(res.size())
       {
            case 1: res="00000"+res;
                    break;
            case 2: res="0000"+res;
                    break;
            case 3: res="000"+res;
                    break;
            case 4: res="00"+res;
                    break;
            case 5: res="0"+res;
                    break;
       }
   }
   return res;
};

string PostFix::toUpperCase(string key)
{
    string upperKey="";
    for(int i = 0 ; i < key.length(); i++)
    {   upperKey.push_back(toupper(key[i]));    }
    return upperKey;
};





