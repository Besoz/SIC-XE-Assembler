#ifndef POSTFIX_H_INCLUDED
#define POSTFIX_H_INCLUDED

#include<unordered_map>
#include<string>
#include<vector>

using namespace std;

class PostFix
{
    private:
            int lowerBound,upperBound,type;
            string address;
            vector<string> postFix;
            vector<string> generateInfix(string operand);
            int computeType(int secondType,int firstType, char operation);
            int hexStringToDecimel(string hexaString);
            string DecimalTohexString(int decimel);
            string toUpperCase(string key);
            bool checkBrackets(string operand);

    public:
            void infixToPostfix(string operand);
            pair<string,int>  evaluatePostfix(unordered_map<string , pair<string,int> > *symTable, string locationCounter);
};
#endif // POSTFIX_H_INCLUDED
