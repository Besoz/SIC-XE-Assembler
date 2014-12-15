#include "ObjectCode.h"
#include "Instruction.h"
#include "writer.h"
#include "PostFix.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <deque>

using namespace std;

ObjectCode::ObjectCode(string length,string addressOfFirstInstr,string path){
    writerObject.createFile(path+"ObjectFile.txt");
    line = "";
    startingAddress = "";
    lineLength = 0;
    MaxLength = 30;
    lenghtOfProgram = length;
    addressOfFirstInstruction = addressOfFirstInstr;
    registerNumbers["A"] = '0';
    registerNumbers["X"] = '1';
    registerNumbers["L"] = '2';
    registerNumbers["B"] = '3';
    registerNumbers["S"] = '4';
    registerNumbers["T"] = '5';
    registerNumbers["F"] = '6';
    registerNumbers["PC"] = '8';
    registerNumbers["SW"] = '9';
};

string ObjectCode::addRecord(Instruction* instruct,string operandLC,string OPCode,int format,bool relocatable){
    if(instruct->getOperation() == "START"){
        // write Header Record
        createHeaderRecond(instruct->getLabel());
        return "START OBJECTCODE";
    }else if(instruct->getOperation() == "END"){
        // write End Record
        writeLine();
        writeModificationRecords();
        createEndRecord();
        return "END OBJECTOCDE";
    }else if((instruct->getOperation()).at(0)=='='){
        char ch = (instruct->getOperation()).at(1);
        string code = "";
        int length = 0;
        if(ch == 'C'){
            for(int i=3 ; i<(instruct->getOperation()).length() -1 ; i++ ){
                int x = (instruct->getOperation()).at(i) - 0;
                code.append(DecimalTohexString(x,false));
                length++;
            }
        }else if(ch == 'X'){
            for(int i=3 ; i<(instruct->getOperation()).length() -1 ; i++ ){
                code.push_back((instruct->getOperation()).at(i));
                length++;
            }
            length = length/2;
        }else{
            cout <<"Invalid Literal"<<endl;
        }
        createTextRecond(code,length,instruct->getLocation());
        return code;
    }else if(instruct->getOperation() == "BYTE"){
        string code = "";
        int length = 0;
        if((instruct->getOperand()).at(0)=='C'){
            for(int i=2 ; i<(instruct->getOperand()).length() -1 ; i++ ){
                int x = (instruct->getOperand()).at(i) - 0;
                code.append(DecimalTohexString(x,false));
                length++;
            }
        }else{
            for(int i=2 ; i<(instruct->getOperand()).length() -1 ; i++ ){
                code.push_back((instruct->getOperand()).at(i));
                length++;
            }
            length = length/2;
        }
        createTextRecond(code,length,instruct->getLocation());
        return code;
    }else if(instruct->getOperation() == "WORD"){//operand is in decimal(string).
        //first convert the decimal string to decimal int
        int val;
        char str[20];
        strcpy(str, (instruct->getOperand()).c_str());
        val = atoi(str);
        //then convert the decimal int to hex string.
        string code = DecimalTohexString(val,true);
        createTextRecond(code,3,instruct->getLocation());
        return code;
    }else if(instruct->getOperation() == "RSUB"){
        createTextRecond("4F0000",3,instruct->getLocation());
        return "4F0000";
    }else{
        if(format == 2){
            string operand = instruct->getOperand();
            string code = OPCode;
            stringstream ss;
            stringstream ss2;
            string reg1="";
            ss<<operand.at(0);
            ss>>reg1;
            string reg2="";
            if(operand.length() > 2){
                ss2<<operand.at(2);
                ss2>>reg2;
                code.push_back(registerNumbers[reg1]);
                code.push_back(registerNumbers[reg2]);
            }else{
                code.push_back(registerNumbers[reg1]);
                code.push_back('0');
            }
            createTextRecond(code,2,instruct->getLocation());
            return code;
        }else{
            if((instruct->getFlagN() && instruct->getFlagI())||(!(instruct->getFlagN()) && !(instruct->getFlagI()))){
                string codeOfOperation = "0";
                string code1 = DecimalTohexString(hexStringToDecimel(OPCode) + 3,false);
                if(code1.length() == 1){
                    codeOfOperation.append(code1);
                }else{
                    codeOfOperation = code1;
                }
                string newOperandLC = operandLC;
                int val;
                char str[20];
                strcpy(str, (operandLC.c_str()));
                val = atoi(str);
                if(val > 1048575 || val < -1048576){//Max Range format 4
                    return "OverFlow Error";
                }
                if(!relocatable){
                    newOperandLC=DecimalTohexString(val,true);
                }
                if(instruct->getFlagE()){//format 4 , no need to computations
                    string code = "";
                    if(instruct->getFlagX()){
                        newOperandLC[0]='9';
                        code.append(codeOfOperation);
                        code.append(newOperandLC);
                    }else{
                        newOperandLC[0]='1';
                        code.append(codeOfOperation);// doing append because we need 8 characters
                        code.append(newOperandLC);
                    }
                    if(relocatable){
                        createModificationRecord(instruct->getLocation(),5);
                    }
                    createTextRecond(code,4,instruct->getLocation());
                    return code;
                }else{// format 3
                    if(instruct->getFlagX()){
                        if(!relocatable)    {//absolute , no computation
                            if(val > 4095 || val < -4096){
                                return "OverFlow Error";
                            }
                            string code(newOperandLC);
                            code[2]='8';
                            if(codeOfOperation.length() > 1 && code.length() > 5){
                                code[0] = codeOfOperation[0];
                                code[1] = codeOfOperation[1];
                            }
                            createTextRecond(code,3,instruct->getLocation());
                            return code;
                        }else{// relative
                            //get the pc counter in hex string and i want to convert to decimal int.
                            int CurrentPC =  hexStringToDecimel(instruct->getLocation()) + 3;
                            int operandLocationCounter = hexStringToDecimel(newOperandLC);
                            int displacement = operandLocationCounter - CurrentPC;
                            if(displacement > 2047 || displacement < -2048){
                                return "OverFlow Error";
                            }
                            string dis = DecimalTohexString(displacement,true);
                            string finalDisplacement = "0000";
                            int counter = 3;
                            int i = dis.length() - 1;
                            while(counter >= 0 && i>=0 ){
                                finalDisplacement[counter] = dis[i];
                                counter--;
                                i--;
                            }
                            finalDisplacement[0] = 'A';
                            string code = "";
                            code.append(codeOfOperation);
                            code.append(finalDisplacement);
                            createTextRecond(code,3,instruct->getLocation());
                            return code;
                        }
                    }else{
                        if(!relocatable){//no computation
                            if(val > 4095 || val < -4096){
                                return "OverFlow Error";
                            }
                            string code(newOperandLC);
                                code[2] = '0';
                            if(code.length() > 5 && codeOfOperation.length() > 1){
                                code[0] = codeOfOperation[0];
                                code[1] = codeOfOperation[1];
                            }
                            createTextRecond(code,3,instruct->getLocation());
                            return code;
                        }else{
                            int CurrentPC =  hexStringToDecimel(instruct->getLocation()) + 3;
                            int operandLocationCounter = hexStringToDecimel(newOperandLC);
                            int displacement = operandLocationCounter - CurrentPC;
                            if(displacement > 2047 || displacement < -2048){
                                return "OverFlow Error";
                            }
                            string dis = DecimalTohexString(displacement,true);
                            string finalDisplacement = "0000";
                            int counter = 3;
                            int i = dis.length() - 1;
                            while(counter >= 0 && i>=0 ){
                                finalDisplacement[counter] = dis[i];
                                counter--;
                                i--;
                            }
                            finalDisplacement[0] = '2';
                            string code = "";
                            code.append(codeOfOperation);
                            code.append(finalDisplacement);
                            createTextRecond(code,3,instruct->getLocation());
                            return code;
                        }
                    }
                }
            }else if(instruct->getFlagN() && !(instruct->getFlagI())){//indirect
               string codeOfOperation = "0";
                string code1 = DecimalTohexString(hexStringToDecimel(OPCode) + 2,false);
                if(code1.length() == 1){
                    codeOfOperation.append(code1);
                }else{
                    codeOfOperation = code1;
                }
                if(instruct->getFlagE()){//format 4
                    string newOperandLC = operandLC;
                    int val;
                    char str[20];
                    strcpy(str, (operandLC.c_str()));
                    val = atoi(str);
                    if(val > 1048575 || val < -1048576){
                        return "OverFlow Error";
                    }
                    if(!relocatable){//convert the decimal int to hex string.
                        newOperandLC=DecimalTohexString(val,true);
                    }
                    newOperandLC[0]='1';
                    string code = "";
                    code.append(codeOfOperation);
                    code.append(newOperandLC);
                    if(relocatable){
                        createModificationRecord(instruct->getLocation(),5);
                    }
                    createTextRecond(code,4,instruct->getLocation());
                    return code;
                }else{//format 3
                    if(relocatable){//relative
                        int CurrentPC = hexStringToDecimel(instruct->getLocation()) + 3;
                        int operandLocationCounter = hexStringToDecimel(operandLC);
                        int displacement = operandLocationCounter - CurrentPC;
                        if(displacement > 2047 || displacement < -2048){
                                return "OverFlow Error";
                        }
                        string dis = DecimalTohexString(displacement,true);
                        string finalDisplacement = "0000";
                        int counter = 3;
                        int i = dis.length() - 1;
                        while(counter >= 0 && i>=0 ){
                            finalDisplacement[counter] = dis[i];
                            counter--;
                            i--;
                        }
                        finalDisplacement[0]='2';
                        string code = "";
                        code.append(codeOfOperation);
                        code.append(finalDisplacement);
                        createTextRecond(code,3,instruct->getLocation());
                        return code;
                    }else{//absolute
                        string newOperandLC = "";
                        int val;
                        char str[20];
                        strcpy(str, (operandLC.c_str()));
                        val = atoi(str);
                        if(val > 4095 || val < -4096){
                            return "OverFlow Error";
                        }
                        //convert the decimal int to hex string.
                        newOperandLC=DecimalTohexString(val,true);
                        newOperandLC[0] = codeOfOperation[0];
                        newOperandLC[1] = codeOfOperation[1];
                        newOperandLC[2] = '0';
                        createTextRecond(newOperandLC,3,instruct->getLocation());
                        return newOperandLC;
                    }
                }
            }else if(!(instruct->getFlagN()) && instruct->getFlagI()){//immediate
                string codeOfOperation = "0";
                string code1 = DecimalTohexString(hexStringToDecimel(OPCode) + 1,false);
                if(code1.length() == 1){
                    codeOfOperation.append(code1);
                }else{
                    codeOfOperation = code1;
                }
                if(instruct->getFlagE()){//format 4
                    string newOperandLC = operandLC;
                    int val;
                    char str[20];
                    strcpy(str, (operandLC.c_str()));
                    val = atoi(str);
                    if(val > 1048575 || val < -1048576){
                        return "OverFlow Error";
                    }
                    if(!relocatable){//convert decimal string to hex string
                        //convert the decimal int to hex string.
                        newOperandLC=DecimalTohexString(val,true);
                    }
                    newOperandLC[0]='1';
                    string code = "";
                    code.append(codeOfOperation);
                    code.append(newOperandLC);
                    if(relocatable){
                        createModificationRecord(instruct->getLocation(),5);
                    }
                    createTextRecond(code,4,instruct->getLocation());
                    return code;
                }else{//format 3
                    if(relocatable){//relative
                        int CurrentPC = hexStringToDecimel(instruct->getLocation()) + 3;
                        int operandLocationCounter = hexStringToDecimel(operandLC);
                        int displacement = operandLocationCounter - CurrentPC;
                        if(displacement > 2047 || displacement < -2048){
                                return "OverFlow Error";
                            }
                        string dis = DecimalTohexString(displacement,true);
                        string finalDisplacement = "0000";
                        int counter = 3;
                        int i = dis.length() - 1;
                        while(counter >= 0 && i>=0 ){
                            finalDisplacement[counter] = dis[i];
                            counter--;
                            i--;
                        }
                        finalDisplacement[0]='2';
                        string code = "";
                        code.append(codeOfOperation);
                        code.append(finalDisplacement);
                        createTextRecond(code,3,instruct->getLocation());
                        return code;
                    }else{//absolute
                        string newOperandLC = "";
                        int val;
                        char str[20];
                        strcpy(str, (operandLC.c_str()));
                        val = atoi(str);
                        if(val > 4095 || val < -4096){
                            return "OverFlow Error";
                        }
                        //convert the decimal int to hex string.
                        newOperandLC=DecimalTohexString(val,true);
                        newOperandLC[0] = codeOfOperation[0];
                        newOperandLC[1] = codeOfOperation[1];
                        newOperandLC[2] = '0';
                        createTextRecond(newOperandLC,3,instruct->getLocation());
                        return newOperandLC;
                    }
                }
            }else{
                 return "";
            }
        }
    }
};

void ObjectCode::createHeaderRecond(string nameOfProgram){
    line = "";
    string address(addressOfFirstInstruction);
    line.append("H^");
    line.append(nameOfProgram.append("^"));
    line.append(address.append("^"));
    line.append(lenghtOfProgram);
    writerObject.writeToFile(line);
    line = "";
    startingAddress = "";
    lineLength = 0;
};

void ObjectCode::createEndRecord(){
    line = "";
    line.append("E^");
    line.append(addressOfFirstInstruction);
    writerObject.writeToFile(line);
};


int ObjectCode::hexStringToDecimel(string hexaString)
{
    int x;
    stringstream stream;
    stream <<hex<< hexaString;
    stream >> x;
    return x;
};

string ObjectCode::DecimalTohexString(int decimel,bool spaces)
{
   stringstream stream;
   stream<<hex<<decimel;
   string res (stream.str());
   res = toUpperCase(res);
   if(res.size()<6 && spaces)
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


string ObjectCode::toUpperCase(string key)
{
    string upperKey="";
    for(int i = 0 ; i < key.length(); i++)
    {   upperKey.push_back(toupper(key[i]));    }
    return upperKey;
};

void ObjectCode::createModificationRecord(string address, int length){
    string ModifiedRecord = "M^";
    int IntAddress = hexStringToDecimel(address) +1;
    string hexAddress = DecimalTohexString(IntAddress,true);
    ModifiedRecord.append(hexAddress.append("^"));
    string len = DecimalTohexString(length,false);
    if(len.length() == 1){
        string len2 = "0";
        len2.append(len);
        len = len2;
    }
    ModifiedRecord.append(len);
    ModificationRecords.push_back(ModifiedRecord);
};

void ObjectCode::printModificationRecords(){
    for (deque<string>::iterator it = ModificationRecords.begin(); it!=ModificationRecords.end(); ++it)
        cout<< *it<<endl;
};

void ObjectCode::writeModificationRecords(){
    for (deque<string>::iterator it = ModificationRecords.begin(); it!=ModificationRecords.end(); ++it){
        writerObject.writeToFile(*it);
    }
};

void ObjectCode::createTextRecond(string record,int len,string address){
    if(lineLength <= (MaxLength-len) ){
        if(lineLength == 0){
            startingAddress = string(address);
        }
        (line.append("^")).append(record);
        lineLength = lineLength + len;
    }else{
        writeLine();
        startingAddress = string(address);
        (line.append("^")).append(record);
        lineLength = lineLength + len;
    }
};

void ObjectCode::writeLine(){
    if(lineLength==0){
        return;
    }
    string lineToWrite = "";
    lineToWrite.append("T^");// append T
    lineToWrite.append(startingAddress.append("^"));// append address Of first Instruction
    string number1 = "0";
    string number2 = DecimalTohexString(lineLength,false);
    if(number2.length() == 1){
        number1.append(number2);
    }else{
        number1 = number2;
    }
    lineToWrite.append(number1);
    lineToWrite.append(line);
    writerObject.writeToFile(lineToWrite);
    line = "";
    lineLength = 0;
    startingAddress = "";
};
