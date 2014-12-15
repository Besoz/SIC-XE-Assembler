#include <windows.h>
#include <iostream>
#include "Pass2.h"
using namespace std;

char* GetDesktopPath(void){
    ULONG ulDataType;
    HKEY hKey;
    DWORD dwToRead = 100;
    static char strPath[100];
    char strKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
    RegOpenKeyEx(HKEY_CURRENT_USER,strKey,0,KEY_READ,&hKey);
    RegQueryValueEx(hKey,"Desktop",NULL,&ulDataType,(LPBYTE)strPath,&dwToRead);
    strPath[dwToRead] = '\0';
    RegCloseKey(hKey);
    return strPath;
}

string getDeskTop(){
    string input(GetDesktopPath());
    string path = "";
    for(int i=0 ; i<input.length() ; i++){
        if(input[i]=='\\'){
            path.push_back('\\');
        }
        path.push_back(input[i]);
    }
    path.append("\\\\");
    return path;
}

int main(int argv, char *argc[])
{
    string path = getDeskTop();
    int choice;
    cout<<endl<<"Choose Format:\n";
    cout<<" 1) Fixed-Format\n";
    cout<<" 2) Free-Format\n";
    cout<<"Choice: ";
    cin>>choice;
    //Pass2 a(path,argc[1],(choice==2));
    Pass2 a(path,"read.txt",(choice==2));
    a.run(path);
    cout << "Done!" << endl;
    return 0;
}
