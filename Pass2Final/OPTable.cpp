#include "OPTable.h"
#include "reader.h"
#include <iostream>
#include <map>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <ctype.h>

using namespace std;

OPTable::OPTable(string path)
{
    initailizer(path);
};

void OPTable::initailizer(string path){
    reader opReader;
    opReader.openFile(path);
    string line;
    while(!opReader.myfile.eof())
    {
        line = opReader.getNext();
        int length = 0;
        for(;length<15 ;length++)
        {
            if(line.at(length)==' ')    break;
        }
        string mnemonic = line.substr(0,length);
        int form = line.at(15)-'0';
        string opcode = line.substr(24,2);
        table[mnemonic]=make_pair(form,string(opcode));
    }
};

int OPTable::getFormat(string key)
{
    return table[toUpperCase(key)].first;
};

string OPTable::getOPCode(string key){
    return table[toUpperCase(key)].second;
};

string OPTable::toUpperCase(string key)
{
    string upperKey="";
    for(int i = 0 ; i < key.length(); i++)
    {   upperKey.push_back(toupper(key[i]));    }
    return upperKey;
};

