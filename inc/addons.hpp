#ifndef _addons_h_
#define _addons_h_

#include <stdlib.h>
#include<vector>
#include<string>
#include<map>


using namespace std;

typedef struct {
  string name;
  string loc;
  string flag;
  string section;
  int numSec;
}SymDef;

typedef struct {
  vector<SymDef*> tabela;
  char* file;
  map<string,string> literalPool;
  int endloc;
  unsigned int entry =0;
  int curSec;
}SymTab;

typedef struct {
  unsigned int hdrSize;
  unsigned int shtStart;
  unsigned short shtNumEntries;
  unsigned int zero;
}HeaderInfo;

typedef struct{
  unsigned int offsTab;
  unsigned int start;
  unsigned int size;
  unsigned int zero;

}SectionDef;

typedef struct{
  string name;
  SectionDef data;
}SectionVec;

typedef struct{
  unsigned int offs;
  char info;
  char toResolve;
  unsigned short section;
  unsigned int val;
  unsigned int zero;
}InputSymbol;



#endif