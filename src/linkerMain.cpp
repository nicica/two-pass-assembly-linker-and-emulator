#include "../inc/linker.hpp"
#include "../inc/linkerArgs.hpp"
#include <iostream>
#include <cstring>


using namespace std;

int main(int argc,char** argv){

  
  int indH=isIn(argv,"-hex",argc);
  int out=checkOut(argc,argv);
  int objStart;
  int on=objNum(argc,argv,objStart);
  int placeStart;
  int place=placeArgs(argc,argv,placeStart);

  if(indH<0 || out<0 || on<=0 || place<0){
    cerr<< "Invalid arguments" <<endl;
    return -1;
  }
  string* input = new string[on];
  for(int i=0;i<on;i++){
    input[i]=argv[i+objStart];
  }


  string name;
  int addr;
  Linker *l= new Linker(argv[out],input,on);
  for(int i=0;i<place;i++){
    if(extractPlace(name,addr,argv[i+placeStart])<0)
      {
        cerr<<"Invalid placement argument"<<endl;
        return -1;
      }
    if(l->addPlacement(addr,name)<0)
      return -1;
  }

  l->start_linking();
  delete l;
  delete[] input;
  return 0;
}