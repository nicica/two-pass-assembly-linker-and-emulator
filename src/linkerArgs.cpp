#include "../inc/linkerArgs.hpp"




int isIn(char **args,string argument,int num){
  int ret=-1;
  bool found=false;
  string cur;
  for(int i=0;i<num;i++){
    cur=args[i];
    if(cur==argument && found){
      return -1;
    }
    if(cur==argument && !found){
      ret=i;
      found= true;
    }
  }
  return ret;
}

int checkOut(int argc,char** argv){
  int ind=isIn(argv,"-o",argc);
  if(ind<0 || ind==(argc-1))
    return -1;
  string out=argv[ind+1];
  if(out.substr(out.length()-4)!=".hex")
    return -1;

  return ind+1;
}

int objNum(int argc,char** argv,int &first){
  first=1;
  int num=0;
  string cur;
  bool stopped=false;
  for(int i=1;i<argc;i++){
    cur=argv[i];
    if(stopped && cur.substr(cur.length()-2)==".o")
      return -1;
    if(num==0 && cur.substr(cur.length()-2)==".o"){
      num++;
      first=i;
    }
    else if (num>0 && cur.substr(cur.length()-2)==".o")
    {
      num++;
    }
    else if(num>0 && cur.substr(cur.length()-2)!=".o")
    {
      stopped=true;

      if(cur!="-hex" && cur!="-o" && cur.compare(0,7,"-place=",7)!=0 && cur.substr(cur.length()-4)!=".hex")
        return -1;
    }
  }

  return num;
}

int placeArgs(int argc,char** argv,int &first){
  first=0;
  int num=0;
  string cur;
  bool stopped=false;
  for(int i=1;i<argc;i++){
    cur=argv[i];
    if(stopped && cur.compare(0,7,"-place=",7)==0)
      return -1;
    if(num==0 && cur.compare(0,7,"-place=",7)==0){
      num++;
      first=i;
    }
    else if (num>0 && cur.compare(0,7,"-place=",7)==0)
    {
      num++;
    }
    else if(num>0 && cur.compare(0,7,"-place=",7)!=0)
    {
      stopped=true;
      if(cur!="-hex" && cur!="-o" && cur.substr(cur.length()-2)!=".o" && cur.substr(cur.length()-4)!=".hex")
        return -1;
    }
  }

  return num;
}

int extractPlace(string& name,int& addr,string place){

  string sub = place.substr(7);  // Ignore first 7 characters

    int found = sub.find('@');
    if (found == string::npos) {
        return -1;  // Invalid format
    }
    
    name = sub.substr(0, found);             // Extract the string part
    string hexValue = sub.substr(found + 1);       // Extract the hex value part
    // Validate the hex value
    if (!(hexValue.compare(0, 2,"0x",2) == 0
      && hexValue.size() > 2
      && hexValue.find_first_not_of("0123456789abcdefABCDEF", 2) == string::npos 
      && hexValue.size()<=10
      )){
      return -1;
    }

    addr=stoul(hexValue.c_str(),nullptr,16);

    
  return 0;
}