#ifndef _linker_args_h_
#define _linker_args_h_
#include<string>
#include <iostream>
#include<cstring>

using namespace std;

int isIn(char **args,string argument,int num);

int checkOut(int argc,char** argv);

int objNum(int argc,char** argv,int &first);

int placeArgs(int argc,char** argv,int &first);

int extractPlace(string& name,int& addr,string place);


#endif