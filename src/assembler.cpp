#include "../inc/assembler.hpp"
#include "../inc/addons.hpp"
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;




static SymTab* simboli=nullptr;
static vector<int> starts;
static int pass;
static unsigned int locationCounter=0;
static ofstream objFile;
static HeaderInfo header;
static vector<SectionVec> sekcije;
static SectionVec cur;
static map<string,bool> passed;



//Helper functions


string convert(int num){
  string ret="0x";
  char hexnum[8];
  sprintf(hexnum,"%X",num);
  ret+=hexnum;
  return ret;
}
void init_tabs(){

  addZeros((16-objFile.tellp()%16));
  
  //symtab
  sekcije.at(0).data.start=objFile.tellp();
 // addZeros(1);
  InputSymbol is;
  int pom=1;
  for(int i=0;i<simboli->tabela.size();i++){
    is={0,0,0,0,0,0};
    if(simboli->tabela.at(i)->flag=="gd" || simboli->tabela.at(i)->flag=="g" || simboli->tabela.at(i)->flag=="e")
      is.info=1;
    else if (simboli->tabela.at(i)->flag=="und")
      is.info=-1;
    if (simboli->tabela.at(i)->flag=="und" || simboli->tabela.at(i)->flag=="e")
      is.toResolve=1;
    is.section=simboli->tabela.at(i)->numSec;
    is.offs=pom;
    is.val=stoul(simboli->tabela.at(i)->loc, nullptr, 16)-(is.section>3 && simboli->tabela.at(i)->flag!="und" && simboli->tabela.at(i)->flag!="e" ?
    starts.at(is.section-4) : 0 );
    objFile.write((const char*)(&is),sizeof(InputSymbol));
    pom+=(simboli->tabela.at(i)->name.length()+1);
  }
  sekcije.at(0).data.size=objFile.tellp()-sekcije.at(0).data.start;
  addZeros((16-objFile.tellp()%16));


  //strtab
  sekcije.at(1).data.start=objFile.tellp();
  addZeros(1);
  for(int i=0;i<simboli->tabela.size();i++){
    objFile.write(simboli->tabela.at(i)->name.c_str(),simboli->tabela.at(i)->name.length()+1);
  }
  sekcije.at(1).data.size=objFile.tellp()-sekcije.at(1).data.start-1;
  //shrstab
  addZeros((16-objFile.tellp()%16));
  sekcije.at(2).data.start=objFile.tellp();
  addZeros(1);
  for(int i=0;i<sekcije.size();i++){
    objFile.write(sekcije.at(i).name.c_str(),sekcije.at(i).name.length()+1);
  }
  sekcije.at(2).data.size=objFile.tellp()-sekcije.at(2).data.start-1;
  addZeros((16-objFile.tellp()%16));


  header.shtNumEntries=sekcije.size();
  header.shtStart=objFile.tellp();
  objFile.seekp(0);
  objFile.write((const char*)(&header),sizeof(HeaderInfo));
  objFile.seekp(0,ios::end);


  SectionDef zeroSec={0,0,0,0};
  objFile.write((const char*)(&zeroSec),sizeof(SectionDef));
  for(int i=0;i<sekcije.size();i++){
    objFile.write((const char*)(&sekcije.at(i).data),sizeof(SectionDef));
  }
}
void init_rela(){

}
void init_secs(){
  sekcije.push_back({".symtab",{0x1,0,0,0}});
  sekcije.push_back({".strtab",{0x9,0,0,0}});
  sekcije.push_back({".shstrtab",{0x11,0,0,0}});
}





void start_asm(char *myFile,char* newFile){

  objFile.open(newFile,ios::binary);
  simboli=new SymTab();
  int len=strlen(myFile);
  simboli->file= new char[len+1];
  simboli->file=myFile;
  simboli->curSec=3;
  pass=1;
  init_secs();
  header.hdrSize=0x10;
  header.shtNumEntries=3;
  header.shtStart=0;
  objFile.write((const char*)(&header),sizeof(HeaderInfo));
  cur.name=" ";
  //yyparse();
}
void reset_asm(){
  objFile.close();
  pass=1;
  simboli=nullptr;
}
void changePass(){
  pass=2;
  locationCounter=0;
}
int getPass(){
  return pass;
}

void incLoc(int val){
  locationCounter+=val;
}

int getLoc(){
  return locationCounter;
}

int addSym(char* name,char* loc,char* flg){
  string flag=flg;
  for(int i=0;i<simboli->tabela.size();i++){
    if(name==simboli->tabela.at(i)->name && flag==simboli->tabela.at(i)->flag)
      {return -1;}
    else if(name==simboli->tabela.at(i)->name && flag!=simboli->tabela.at(i)->flag){
      if(simboli->tabela.at(i)->flag=="gd")
        {return -1;}
      if(flag=="l" && simboli->tabela.at(i)->flag=="g") {
        simboli->tabela.at(i)->flag="gd";
        simboli->tabela.at(i)->loc=loc;
        simboli->tabela.at(i)->section=cur.name;
        simboli->tabela.at(i)->numSec=simboli->curSec > 3 ? simboli->curSec : 0;
        return 0;
      }
      else if(flag=="g" && simboli->tabela.at(i)->flag=="l")
      {
        simboli->tabela.at(i)->flag="gd";
        return 0;
      }
      else
        {return -1;}
    }
  }
  
  SymDef *sd= new SymDef();
  sd->name=name;
  sd->loc=loc;
  sd->flag=flag;
  sd->section=cur.name;
  sd->numSec=simboli->curSec > 3 ? simboli->curSec : 0;
  simboli->tabela.push_back(sd);
  //delete sd;
  return 0;
}

void addLiteral(char* l){
  string elem=l;
  if(getPass()==1){
  if (simboli->literalPool.find(elem) == simboli->literalPool.end()) {
    simboli->literalPool.insert({elem,to_string(simboli->entry)});
    simboli->entry++;
    passed.insert({elem,false});
  }}
  if(getPass()==2){
    if(passed[elem])
      return;
    int newVal=(stoi(simboli->literalPool[elem]))*4+simboli->endloc;
    simboli->literalPool[elem]=convert(newVal);
    passed[elem]=true;
    
  }

}

void printSymTab(){
  if(simboli->tabela.size()>0){
  for (int i=0;i<simboli->tabela.size();i++) {
      cout << simboli->tabela.at(i)->name << " "<< simboli->tabela.at(i)->loc 
      << " "<< simboli->tabela.at(i)->flag << " "<< simboli->tabela.at(i)->section;
    cout << endl;
  }
  }
  cout<<endl;
}
void printLiteralPool(){
  for(const auto& elem : simboli->literalPool)
  {
    cout << elem.first << " " << elem.second << "\n";
  }
  cout<<endl;
}
void set_end(){
  simboli->endloc=locationCounter;
}
void writeCode(unsigned int code){
  objFile.write((const char*)(&code),sizeof(int));
  
}
int getRegIndex(char* reg){
  string r=reg;
  if(r=="%status")
    return 0;
  else if(r=="%handler")
    return 1;
  else if(r=="%cause")
    return 2;
  else if(r=="%sp")
    return 14;
  else if (r=="%pc")
    return 15;
  else{
  r.erase(0,2);

  return stoi(r);}
}

void addZeros(int num){
  short zero=0;
  for (int i=0;i<num;i++){
    objFile.write((const char*)(&zero),sizeof(char));
  }
}

unsigned int getLitAddr(char *lit){
  return (unsigned int)strtol(simboli->literalPool[lit].c_str(), NULL, 0);
 
}

unsigned int getSymAddr(char* lit){
  string l=lit;
  bool found=false;
  int i;
  for(i=0;i<simboli->tabela.size();i++){
    if(simboli->tabela.at(i)->name==l)
      {
        found=true;
        break;
      }
  }
  if(!found){
    SymDef *sd= new SymDef();
    sd->name=l;
    sd->flag="und";
    sd->loc="0x0";
    sd->section=cur.name;
    sd->numSec=simboli->curSec > 3 ? simboli->curSec : 0;
    simboli->tabela.push_back(sd);
  }
  int num=4*simboli->literalPool.size()+simboli->endloc;
  num+=(16-num%16);
  return num+i*16+8;
}

void init_file(){
  if(cur.name!=" ")
    sekcije.erase(sekcije.end());
 int i=0;
 for (int i=0;i<simboli->literalPool.size();i++)
 {
  for (const auto& pair : simboli->literalPool) {
        if (pair.second == convert(i*4+simboli->endloc)) {
            int orCode;
            if(pair.first[0]=='0' && pair.first[1]=='x')
              orCode =stoul(pair.first,nullptr,16);
            else
              orCode =stoi(pair.first);
            writeCode(orCode); 
        }
 } 
}
init_rela();
init_tabs();
}

int getNum(char *lit){
  if(lit[0]=='0' && lit[1]=='x')
    return stoul(lit, nullptr, 16);
  else 
    return stoi(lit);
}

int setCurrSec(char* name){
  simboli->curSec++;
    setCur();
  string n=name;
  for(int i=0;i<sekcije.size();i++){
    if(sekcije.at(i).name==n)
      return -1;
  }
  if(n==cur.name)
    return -1;
  unsigned int of=sekcije.back().data.offsTab+sekcije.back().name.length();
  cur={n,{of+1,locationCounter+header.hdrSize,0,0}};
  starts.push_back(locationCounter);
  return 0;
}

void setCur(){
  if(cur.name!=" "){
  cur.data.size=locationCounter-(cur.data.start-header.hdrSize);
  sekcije.push_back(cur);}
}
int getOffs(){
  return locationCounter-(cur.data.start-header.hdrSize);
}