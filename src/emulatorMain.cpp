#include "../inc/emulator.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc,char** argv){
  if(argc!=2){
    cerr<<"Invalid Arguments"<<endl;
    return -1;
  }
  string s=argv[1];
  if(s.substr(s.length()-4)!=".hex"){
    cerr<<"Invalid Arguments"<<endl;
    return -1;
  }
  Emulator* e= new Emulator(s);

  e->emulate();

  

  delete e;
  return 0;
}