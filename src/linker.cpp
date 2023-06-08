#include "../inc/linker.hpp"
#include <iomanip>

Linker::Linker(string out,string* in,int num){
  this->num=num;
  inputFiles= new ifstream[num];
  iFileInfo= new FileData[num];
  for(int i=0;i<num;i++){
    inputFiles[i].open(in[i],ios::binary);
    if(!inputFiles[i]){
      cerr<<"Failed to open file"<<endl;
      return;
    }
  }
  outputFile.open(out,ios::binary);
  if(!outputFile){
    cerr<<"Failed to open file"<<endl;
      return;
  }
}

Linker::~Linker(){
  for(int i=0;i<this->num;i++){
    inputFiles[i].close();
    delete[] iFileInfo[i].sectionNames;
    delete[] iFileInfo[i].sections;
    delete[] iFileInfo[i].symNames;
    delete[] iFileInfo[i].symbols;
    if(i>=3)
      delete[] iFileInfo[i-3].isPlaced;
  }
  outputFile.close();
  delete[] inputFiles;
  delete[] iFileInfo;
}

int Linker::addPlacement(int addr,string name){
  if (sekcije.find(name) == sekcije.end()){
    sekcije.insert({name,addr});
  }
  else
  {
    cerr<<"Multiple section definition error"<<endl;
    return -1;
  }
  return 0;
}

void Linker::start_linking(){
  gatherData();
  
 if(calculatePlacement()<0){
  cerr<<"Placement error"<<endl;
  return;
 }


 if(resolveSymbols()<0){
  return;
 }
  placeData();
  print();
  
}
void Linker::gatherData(){
  for(int i=0;i<num;i++){

    inputFiles[i].get((char*)(&iFileInfo[i].header),sizeof(HeaderInfo));
    //inputFiles[i].seekg(iFileInfo[i].header.shtStart+0x10);
    iFileInfo[i].sections= new SectionDef[iFileInfo[i].header.shtNumEntries];
    iFileInfo[i].sectionNames= new string[iFileInfo[i].header.shtNumEntries];
    iFileInfo[i].isPlaced= new int[iFileInfo[i].header.shtNumEntries-3];
    for(int j=0;j<iFileInfo[i].header.shtNumEntries-3;j++){
      iFileInfo[i].isPlaced[j]=0;
    }
    for(int j=0;j<iFileInfo[i].header.shtNumEntries;j++){
      inputFiles[i].seekg(iFileInfo[i].header.shtStart+(16*(j+1)));
      inputFiles[i].get((char*)(&iFileInfo[i].sections[j]),sizeof(SectionDef));
      
    }
    inputFiles[i].seekg(iFileInfo[i].sections[2].start+1);
    char c;
    string s;
    for(int j=0;j<iFileInfo[i].header.shtNumEntries;j++){
     c=inputFiles[i].get();
     s="";
     while(c!=0){
      s+=c;
      c=inputFiles[i].get();
     } 
    iFileInfo[i].sectionNames[j]=s;
    }
    int numSyms=iFileInfo[i].sections[0].size/16;
    iFileInfo[i].symbols= new InputSymbol[numSyms];
    iFileInfo[i].symNames= new string[numSyms];
    inputFiles[i].seekg(iFileInfo[i].sections[0].start);
    for(int j=0;j<numSyms;j++){
      inputFiles[i].get((char*)(&iFileInfo[i].symbols[j]),sizeof(InputSymbol));
      inputFiles[i].get();
    }
    inputFiles[i].seekg(iFileInfo[i].sections[1].start+1);
    for(int j=0;j<numSyms;j++){
     c=inputFiles[i].get();
     s="";
     while(c!=0){
      s+=c;
      c=inputFiles[i].get();
     }
     iFileInfo[i].symNames[j]=s;
    }
  }
}

int Linker::resolveSymbols(){
  for(int i=0;i<num;i++){
    for(int j=0;j<iFileInfo[i].sections[0].size/16;j++){
      if(iFileInfo[i].symbols[j].info==-1){
        cerr<<"Undefined symbol"<<endl;
        return -1;
      }
      if(iFileInfo[i].symbols[j].toResolve==1){
        if(resolveSymbol(i,iFileInfo[i].symNames[j],iFileInfo[i].symbols[j])<0){
          cerr<<"Uresolved symbol"<<endl;
          return -1;
        }
      }
      else
      {
        if(checkMulDefinitions(i,iFileInfo[i].symNames[j]))
        {
          cerr<<"Multiple symbol definition error"<<endl;
          return -1;
        }
      }
    }
  }

  return 0;
}


int Linker::resolveSymbol(int skip,string name,InputSymbol& symbol){
  for(int i=0;i<num;i++){
    if(i==skip)
      continue;
    for(int j=0;j<iFileInfo[i].sections[0].size/16;j++){
      if(name==iFileInfo[i].symNames[j] && iFileInfo[i].symbols[j].toResolve==0){
        symbol.toResolve=0;
        symbol.val=iFileInfo[i].symbols[j].val;
        return 0;
      }
    }
  }
  return -1;
}

bool Linker::checkMulDefinitions(int skip,string name){
  for(int i=skip+1;i<num;i++){
    for(int j=0;j<iFileInfo[i].sections[0].size/16;j++){
      if(name==iFileInfo[i].symNames[j] && iFileInfo[i].symbols[j].toResolve==0){
        return true;
      }
    }
  }
  return false;
}

int Linker::calculatePlacement(){
  for(const auto& sec:sekcije){
    int f=0,s=3;
    if(findSection(f,s,sec.first)<0){
      return -1;
    }
    unsigned int len=(iFileInfo[f].sections[1].start-16)-iFileInfo[f].sections[s].start ;
    for(int i=0;i<placement_info.size();i++)
    {
      int f1=0,s1=3;
      findSection(f1,s1,placement_info.at(i).name);
      unsigned int end=(iFileInfo[f1].sections[1].start-16)-iFileInfo[f1].sections[s1].start;
      if(sec.second>placement_info.at(i).start && sec.second<placement_info.at(i).start+end)
        return -1;
      if(sec.second+len>placement_info.at(i).start && sec.second<placement_info.at(i).start)
        return -1;
    }
    iFileInfo[f].isPlaced[s-3]=1;
    placement_info.push_back({sec.first,sec.second,sec.second+len});
    
      //cout <<setw(8)<< hex <<sec.first<< " "<< sec.second<< " "<< (sec.second+len)<<endl;
    if(sec.second+len>next_placement)
      next_placement=sec.second+len;
    for(int i=0;i<iFileInfo[f].sections[0].size/16;i++){
      if(iFileInfo[f].symbols[i].section==s+1 && iFileInfo[f].symbols[i].toResolve==0){
        iFileInfo[f].symbols[i].val+=sec.second;
      }
    }

  }


  for(int i=0;i<num;i++){
    for(int j=3;j<iFileInfo[i].header.shtNumEntries;j++){
      if(iFileInfo[i].isPlaced[j-3]==1)
        continue;
      PlaceSection ps;
      if(sectionExists(iFileInfo[i].sectionNames[j],ps))
        {if(rearange(i,j,ps)<0)
          return -1;
        else
          continue;}
      for(int k=0;k<iFileInfo[i].sections[0].size/16;k++){
        if(iFileInfo[i].symbols[k].section==j+1 && iFileInfo[i].symbols[k].toResolve==0){
          iFileInfo[i].symbols[k].val+=next_placement;
      }
    }
      iFileInfo[i].isPlaced[j-3]=1;
      unsigned int len=(iFileInfo[i].sections[1].start-16)-iFileInfo[i].sections[j].start ;
      placement_info.push_back({iFileInfo[i].sectionNames[j],next_placement,next_placement+len});
      //cout <<setw(8)<< hex<<iFileInfo[i].sectionNames[j]<< " "<< next_placement<< " "<< (next_placement+len)<<endl;
      next_placement+=len;
    }
    }

  return 0;
}

int Linker::findSection(int &f,int& s,string name){
  for(int i=f;i<num;i++){
    for(int j=s;j<iFileInfo[i].header.shtNumEntries;j++){
      if(name==iFileInfo[i].sectionNames[j] ){
        f=i;
        s=j;
        return 0;
      }
    }
  }
  return -1;
}

void Linker::placeData(){
  for(int i=0;i<placement_info.size();i++){
    int f=-1,s;
    do{
      f++;
      s=3;
    findSection(f,s,placement_info.at(i).name);
    }while(iFileInfo[f].isPlaced[s-3]==2);
    inputFiles[f].seekg(iFileInfo[f].sections[s].start);
    outputFile.seekp(placement_info.at(i).start);
    while(inputFiles[f].tellg()<iFileInfo[f].sections[0].start){
      char c=inputFiles[f].get();
      outputFile.write(&c,sizeof(char));
    }
    int a=0;
    while(outputFile.tellp()<placement_info.at(i).end){
      outputFile.write((char*)&iFileInfo[f].symbols[a++],sizeof(InputSymbol));
    }
    iFileInfo[f].isPlaced[s-3]=2;
  }
}


void Linker::print(){

  for(const auto& sec:sekcije){
    cout<<endl<<"Section: "<<sec.first<<endl;
    unsigned int start=sec.second;
    int f=0,s=3;
    findSection(f,s,sec.first);
    inputFiles[f].seekg(iFileInfo[f].sections[s].start);
    while(inputFiles[f].tellg()<iFileInfo[f].sections[s].start+iFileInfo[f].sections[s].size){
      cout << hex<<start<<":";
      for(int i=start%8;i<8;i++){
        unsigned char c=inputFiles[f].get();
        if(inputFiles[f].tellg()>iFileInfo[f].sections[s].start+iFileInfo[f].sections[s].size)
          break;
        cout<<" ";
        cout<<setw(2)<<setfill('0') <<hex<<(unsigned int)c;
      }
      cout<<endl;
      start+=(8-start%8);
    }
  }
}


bool Linker::sectionExists(string name,PlaceSection& ps){
  for(int i=0;i<placement_info.size();i++){
    if(placement_info.at(i).name==name)
    {ps=placement_info.at(i);
      return true;}
  }

  return false;
}

int Linker::rearange(int i,int j,PlaceSection ps){
  unsigned int start;
  start=ps.end;
  unsigned int len=(iFileInfo[i].sections[1].start-16)-iFileInfo[i].sections[j].start ;
  
    
    
    iFileInfo[i].isPlaced[j-3]=1;
    for(const auto& sec:sekcije){
      if(sec.first==ps.name)
        continue;
      if(sec.second<start+len && sec.second>start)
        return -1;
    }
    unsigned int a=start;
    int p=0,q=3;
    for(int x=sekcije.size();x<placement_info.size();x++){
      if(ps.name==placement_info.at(x).name)
      {
        start=placement_info.at(x).end;
        continue;
      }
        if(placement_info.at(x).start==a){
          a=placement_info.at(x).end;
      placement_info.at(x).start+=len;
      placement_info.at(x).end+=len;
      
      findSection(p,q,placement_info.at(x).name);
      for(int k=0;k<iFileInfo[p].sections[0].size/16;k++){
        if(iFileInfo[p].symbols[k].section==q+1 && iFileInfo[p].symbols[k].toResolve==0){
          iFileInfo[p].symbols[k].val+=start;
        }
      }   
      p++;
      q=3;
      }
      
    }
    for(int k=0;k<iFileInfo[i].sections[0].size/16;k++){
        if(iFileInfo[i].symbols[k].section==j+1 && iFileInfo[i].symbols[k].toResolve==0){
          iFileInfo[i].symbols[k].val+=start;
      }
    }
    placement_info.push_back({iFileInfo[i].sectionNames[j],start,start+len});

     // cout << iFileInfo[i].sectionNames[j]<< " "<< start<< " "<< (start+len)<<endl;
    next_placement+=len;

  return 0;
}