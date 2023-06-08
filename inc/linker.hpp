#ifndef _linker_h_
#define _linker_h_

#include<iostream>
#include<fstream>
#include<map>
#include "../inc/addons.hpp"
using namespace std;

typedef struct {
  HeaderInfo header;
  SectionDef* sections;
  string* sectionNames;
  string* symNames;
  int* isPlaced;
  InputSymbol* symbols;
}FileData;

typedef struct{
  string name;
  unsigned int start;
  unsigned int end;
}PlaceSection;

class Linker{
  private:
    ifstream* inputFiles;
    ofstream outputFile;
    int num;

    unsigned int next_placement=0;
    vector<PlaceSection> placement_info;

    map<string,unsigned int> sekcije;
    FileData* iFileInfo;

    void gatherData();
    int resolveSymbols();
    int resolveSymbol(int skip,string name,InputSymbol& symbol);
    bool checkMulDefinitions(int skip,string name);
    int calculatePlacement();
    int findSection(int &f,int& s,string name);

    void placeData();
    void print();

    bool sectionExists(string name,PlaceSection &ps);
    int rearange(int i,int j,PlaceSection ps);  
  public:
    Linker(string out,string* in,int num);
    ~Linker();

    int addPlacement(int,string);

    void start_linking();
};


#endif