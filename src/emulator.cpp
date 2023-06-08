#include "../inc/emulator.hpp"
#include<iomanip>

Emulator::Emulator(string s){
  for (int i=0;i<15;i++){
    gpr[i]=0;
  }
  gpr[15]=0x40000000;
  csr[0]=csr[1]=csr[2]=0;
  mem.open(s,ios::binary|ios::in|ios::out);
  if(!mem){
    cerr<<"Cant open file"<<endl;
    return;
  }
  mem.seekg(gpr[15]);
}

void Emulator::emulate(){
  if(!mem){
	return;
  }
  while(true){
    mem.get((char*)&current_instruction,sizeof(unsigned int)+1);
    gpr[15]+=4;
    //cout<<hex<<setw(8)<<setfill('0')<<(gpr[15]-4)<<": "<<setw(8)<<setfill('0')<<current_instruction<<endl;
    if(current_instruction==0){
      halt();
      return;
    }
    unsigned int op_code=((current_instruction) & 0xf0)>>4;
      switch(op_code){
        case 1:
           software_interrupt();
          break;
        case 2:
           call();
          break;
        case 3:
           jump();
          break;
        case 4:
           xchg();
          break;
        case 5:
           a_op();
          break;
        case 6:
           l_op();
          break;
        case 7:
           shift();
          break;
        case 8:
           store();
          break;
        case 9:
           load();
          break;
        default:
        wrong_op_code_intterupt();
          break;
      }
      if(error)
        return;
        
    mem.seekg(gpr[15]);
  }
}
void Emulator::halt(){

  cout<<"-----------------------------------------------------------------"<<endl;
  cout<<"Emulated processor executed halt instruction"<<endl;
  cout<<"Emulated processor state:"<<endl;
  for(int i=0;i<16;i++){
    if(i<10)
      cout<<" ";
    cout<<"r"<<dec<<i<<"=0x";
    cout<<hex<<setw(8)<<setfill('0')<<gpr[i]<<"    ";
    if(i%4==3)
      cout<<endl;
  }
  mem.close();
}



void Emulator::software_interrupt(){
 if(current_instruction!=0x10)
    {wrong_op_code_intterupt();
    return;
    }
  gpr[14]-=4;
  mem.seekp(gpr[14]);
  mem.write((const char*)(&csr[0]),sizeof(int));
  gpr[14]-=4;
  mem.seekp(gpr[14]);
  mem.write((const char*)(&gpr[15]),sizeof(int));
  csr[2]=4;
  csr[0]&=(~0x1);
  gpr[15]=csr[1];
}

void Emulator::call(){
  if(((current_instruction&0xff)!=0x21 && (current_instruction&0xff)!=0x20) || (current_instruction&0xf00000)!=0)
    {
     //cout<< (current_instruction!=0x21) <<" "<< (current_instruction!=0x20)<<" "<<((current_instruction&0xf00000)!=0)<<endl;
      wrong_op_code_intterupt();
    return;
    }
  
  gpr[14]-=4;
  mem.seekp(gpr[14]);
  mem.write((const char*)(&gpr[15]),sizeof(int));
  int d=((current_instruction&0xff000000)>>24) | ((current_instruction&0xf0000)>>8);
  d=(d << 20) >> 20;
  unsigned int b=((current_instruction&0xf00)>>8);
  unsigned int a=((current_instruction&0xf000)>>12);
  unsigned int m=current_instruction & 0xf;
  if(m==0){
    gpr[15]=gpr[a]+gpr[b]+d;
  }
  else{
    mem.seekg(gpr[a]+gpr[b]+d);
        unsigned int t;
        mem.get((char*)(&t),sizeof(int)+1);
        gpr[15]=t;
      
  }
}

void Emulator::jump(){
  unsigned int m=current_instruction & 0xf;
  int d=((current_instruction&0xff000000)>>24) | ((current_instruction&0xf0000)>>8);
  d=(d << 20) >> 20;
  unsigned int c=((current_instruction&0xf00000)>>20);
  unsigned int b=((current_instruction&0xf00)>>8);
  unsigned int a=((current_instruction&0xf000)>>12);
  int sgngpr=gpr[b];
      unsigned int t;
  switch(m){
    case 0: 
      gpr[15]=gpr[a]+d;
      break;
    case 1: 
      if(gpr[b]==gpr[c])
        gpr[15]=gpr[a]+d;
      break;
    case 2: 
      if(gpr[b]!=gpr[c])
        gpr[15]=gpr[a]+d;
      break;
    case 3: 
      if(sgngpr>gpr[c])
        gpr[15]=gpr[a]+d;
      break;
    case 8: 
      mem.seekg(gpr[a]+d);
        mem.get((char*)(&t),sizeof(int)+1);
        gpr[15]=t;
      break;
    case 9: 
      if(gpr[b]==gpr[c]){
        mem.seekg(gpr[a]+d);
        mem.get((char*)(&t),sizeof(int)+1);
        gpr[15]=t;
      }
      break;
    case 10: 
      if(gpr[b]!=gpr[c]){
        mem.seekg(gpr[a]+d);
        mem.get((char*)(&t),sizeof(int)+1);
        gpr[15]=t;}
      break;
    case 11: 
      if(sgngpr>gpr[c]){
        mem.seekg(gpr[a]+d);
        mem.get((char*)(&t),sizeof(int)+1);
        gpr[15]=t;}
      break; 
    default:
      wrong_op_code_intterupt();
      return;
  }
}

void Emulator::xchg(){
  if((current_instruction&0xff0ff0ff)!=0x40){
      wrong_op_code_intterupt();
      return;
  }
  unsigned int c=((current_instruction&0xf00000)>>20);
  unsigned int b=((current_instruction&0xf00)>>8);
  unsigned int temp=gpr[b];
  if(b!=0)
    gpr[b]=gpr[c];
  if(c!=0)
    gpr[c]=temp;
}

void Emulator::a_op(){
  unsigned int m=current_instruction & 0xf;
  if((current_instruction&0xff0f00f0)!=0x50){
      wrong_op_code_intterupt();
      return;
  }
  unsigned int c=((current_instruction&0xf00000)>>20);
  unsigned int b=((current_instruction&0xf00)>>8);
  unsigned int a=((current_instruction&0xf000)>>12);
  switch(m){
    case 0: 
      if(a!=0)
        gpr[a]=gpr[b]+gpr[c];
      break;
    case 1: 
      if(a!=0)
        gpr[a]=gpr[b]-gpr[c];
      break;
    case 2: 
      if(a!=0)
        gpr[a]=gpr[b]*gpr[c];
      break;
    case 3: 
        gpr[a]=gpr[b]/gpr[c];
      break;
    default:
      wrong_op_code_intterupt();
      return;
  }
}

void Emulator::l_op(){
  unsigned int m=current_instruction & 0xf;
  if((current_instruction&0xff0f00f0)!=0x60){
      wrong_op_code_intterupt();
      return;
  }
  unsigned int c=((current_instruction&0xf00000)>>20);
  unsigned int b=((current_instruction&0xf00)>>8);
  unsigned int a=((current_instruction&0xf000)>>12);
  switch(m){
    case 0: 
      if(a!=0)
        gpr[a]=~gpr[b];
      break;
    case 1:
      if(a!=0) 
        gpr[a]=gpr[b]&gpr[c];
      break;
    case 2:
      if(a!=0) 
        gpr[a]=gpr[b]|gpr[c];
      break;
    case 3:
      if(a!=0) 
        gpr[a]=gpr[b]^gpr[c];
      break;
    default:
      wrong_op_code_intterupt();
      return;
  }
}

void Emulator::shift(){
  if((current_instruction&0xff0f00ff)!=0x70 && (current_instruction&0xff0f00ff)!=0x71){
      wrong_op_code_intterupt();
      return;
  }
  unsigned int c=((current_instruction&0xf00000)>>20);
  unsigned int b=((current_instruction&0xf00)>>8);
  unsigned int a=((current_instruction&0xf000)>>12);
  unsigned int m=current_instruction & 0xf;
  if(m==0){
    if(a!=0)
      gpr[a]=gpr[b]<<gpr[c];
  }
  else{
    if(a!=0)
      gpr[a]=gpr[b]>>gpr[c];
  }
}

void Emulator::store(){
  unsigned int m=current_instruction & 0xf;
  int d=((current_instruction&0xff000000)>>24) | ((current_instruction&0xf0000)>>8);
  d=(d << 20) >> 20;
  unsigned int c=((current_instruction&0xf00000)>>20);
  unsigned int b=((current_instruction&0xf00)>>8);
  unsigned int a=((current_instruction&0xf000)>>12);
  switch(m){
    case 0: 
      mem.seekp(gpr[a]+gpr[b]+d);
      mem.write((const char*)(&gpr[c]),sizeof(unsigned int));
      break;
    case 2: 
      mem.seekg(gpr[a]+gpr[b]+d);
      unsigned int temp;
      mem.get((char*)(&temp),sizeof(unsigned int)+1);
      mem.seekp(temp);
      mem.write((const char*)(&gpr[c]),sizeof(unsigned int));
      break;
    case 1: 
      if(a!=0)
        gpr[a]=gpr[a]+d;
      mem.seekp(gpr[a]);
      mem.write((const char*)(&gpr[c]),sizeof(unsigned int));
      break;
    default:
      wrong_op_code_intterupt();
      return;
  }
}

void Emulator::load(){
  unsigned int m=current_instruction & 0xf;
  int d=((current_instruction&0xff000000)>>24) | ((current_instruction&0xf0000)>>8);
  d=(d << 20) >> 20;
  unsigned int c=((current_instruction&0xf00000)>>20);
  unsigned int b=((current_instruction&0xf00)>>8);
  unsigned int a=((current_instruction&0xf000)>>12);
  unsigned int t;
  switch(m){
    case 0: 
      if(b>2){
        wrong_op_code_intterupt();
        return;
      }
      if(a!=0)
        gpr[a]=csr[b];
      break;
    case 1: 
      if(a!=0)
        gpr[a]=gpr[a]+d;
      break;
    case 2: 
      mem.seekg(gpr[b]+gpr[c]+d);
      if(a!=0){
        mem.get((char*)(&t),sizeof(int)+1);
        gpr[a]=t;
      }
      break;
    case 3: 
      mem.seekg(gpr[b]);
      if(a!=0){
        mem.get((char*)(&t),sizeof(int)+1);
        gpr[a]=t;
      }
      if(b!=0)
        gpr[b]=gpr[b]+d;
      break;
    case 4: 
      if(a>2){
        wrong_op_code_intterupt();
        return;
      }
      csr[a]=gpr[b];
      break;
    case 5: 
      if(a>2){
        wrong_op_code_intterupt();
        return;
      }
      csr[a]=gpr[b]+d;
      break;
    case 6: 
      if(a>2){
        wrong_op_code_intterupt();
        return;
      }
      mem.seekg(gpr[b]+gpr[c]+d);
        mem.get((char*)(&t),sizeof(int)+1);
        csr[a]=t;
      break;
    case 7: 
      if(a>2){
        wrong_op_code_intterupt();
        return;
      }
      mem.seekg(gpr[b]);
        mem.get((char*)(&t),sizeof(int)+1);
        csr[a]=t;
      if(b!=0)
        gpr[b]=gpr[b]+d;
      break; 
    default:
      wrong_op_code_intterupt();
      return;
  }
}

void Emulator::wrong_op_code_intterupt(){
  gpr[14]-=4;
  mem.seekp(gpr[14]);
  mem.write((const char*)(&csr[0]),sizeof(int));
  gpr[14]-=4;
  mem.seekp(gpr[14]);
  mem.write((const char*)(&gpr[15]),sizeof(int));
  csr[2]=1;
  csr[0]|=0x4;
  gpr[15]=csr[1];
  error=true;
}