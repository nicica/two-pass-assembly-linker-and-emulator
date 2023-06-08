%{
#include <stdio.h>
#include "inc/assembler.hpp"



%}

%union {
  int val;
  char *name;
}

%token EOFS
%token EOL
%token<name> INS
%token<name> REG
%token<name> OPR
%type<name> exp
%type<name> dir1
%type<name> dir2
%token COMA
%token<name> ASM
%token PLUS
%token LB
%token RB
%token DOL
%token<name> LABEL
%token<name> LIT

%%

input:
| line input 
;

line:
  exp EOL { incLoc(4); }
  | label exp EOL { incLoc(4); }
  | dir1 EOL {free($1);}
  | dir2 EOL {free($1);}
  | EOL
  | EOFS {if(getPass()==1) set_end();return 0;}
  | exp EOFS { if(getPass()==1) set_end(); incLoc(4); return 0; }
  | dir1 EOFS { if(getPass()==1) set_end(); free($1); return 0;}
  | dir2 EOFS {free($1); return 0;};
  | label EOL 
  | label EOFS {if(getPass()==1) set_end(); return 0;}
  ;
label:
  LABEL {
    if(getPass()==1){
      char* hexLoc=malloc(10);
      strcpy(hexLoc,"0x");
      char loc[10];
      sprintf(loc,"%x",getLoc());
      strcat(hexLoc,loc);
      if(addSym($1,hexLoc,"l")!=0){
        free(hexLoc);
        free($1);
        yyerror("multiple symbol definiton error");
        return -1;
      }
    free(hexLoc);
    }   
    free($1);}
  | label LABEL{
    if(getPass()==1){
      char* hexLoc=malloc(10);
      strcpy(hexLoc,"0x");
      char loc[10];
      sprintf(loc,"%x",getLoc());
      strcat(hexLoc,loc);
      if(addSym($2,hexLoc,"l")==-1){
        free(hexLoc);
        free($2);
        yyerror("multiple symbol definiton error");
        return 1;
      }
    free(hexLoc);
    }   
    free($2);}
  ;

exp: INS { 
  if(!strcmp($1,"halt"))
    {
      if(getPass()==2){
        writeCode(0x0);
      }
    }
  else if(!strcmp($1,"int"))
    {
      if(getPass()==2){
        writeCode(0x10);
      }
    }
  else if(!strcmp($1,"iret"))
    {
      if (getPass()==2){

        unsigned int code1=0x04000e96;
        writeCode(code1);
        unsigned int code2=0x0800fe93;
        writeCode(code2);
      }
      incLoc(4);
    }
  else if(!strcmp($1,"ret"))
    {
      if (getPass()==2){
        unsigned int code=0x0400fe93;
        writeCode(code);
      }
    }
  else{
    free($1);
    yyerror("syntax error");
    return -1;
  }
  free($1);
  }
   | INS REG { 
    unsigned int rA=getRegIndex($2);
    if(!strcmp($2,"%cause") || !strcmp($2,"%status") || !strcmp($2,"%handler"))
    {
      free($1); free($2); 
      yyerror("syntax error");
      return -1;
    }
    if(!strcmp($1,"push"))
    {
      if (getPass()==2){
        unsigned int code=0xfc0fe081;
        code = (code|(rA<<20));
        writeCode(code);
      }
    }
  else if(!strcmp($1,"pop"))
    {
      if (getPass()==2){
        unsigned int code=0x04000e93;
        code = (code|(rA<<12));
        writeCode(code);
      }
    }
  else if(!strcmp($1,"not"))
    {
      if(getPass()==2){
        unsigned int code=0x60;
        code = (code | (rA<<12));
        rA=rA<<8;
        code=(code | rA);
        writeCode(code);
        
      }
    }
  else{
    free($1); free($2); 
    yyerror("syntax error");
    return -1;
  }
    free($1); free($2); 
    }
   | INS REG COMA REG {
    unsigned int rA=getRegIndex($2);
    unsigned int rB=getRegIndex($4);
    if(!strcmp($2,"%cause") || !strcmp($2,"%status") || !strcmp($2,"%handler"))
    {
      if(!strcmp($1,"csrrd"))
      {
        if(getPass()==2){
        unsigned int code=0x90;
        rA=rA<<8;
        rB=rB<<12;
        code=(code | rA | rB);
        writeCode(code);
        
      }
      }
      else{
      free($1); free($2); free($4);
      yyerror("syntax error");
      return -1;
      }
    }
    else if(!strcmp($4,"%cause") || !strcmp($4,"%status") || !strcmp($4,"%handler"))
    {
      if(!strcmp($1,"csrwr"))
      {
        if(getPass()==2){
        unsigned int code=0x94;
        rA=rA<<8;
        rB=rB<<12;
        code=(code | rA | rB);
        writeCode(code);
        
      }
      }
      else{
      free($1); free($2); free($4);
      yyerror("syntax error");
      return -1;
      }
    }else{
  if(!strcmp($1,"xchg"))
    {
      if(getPass()==2){
        unsigned int code=0x40;
        rA=rA<<8;
        rB=rB<<20;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"add"))
    {
      if(getPass()==2){
        unsigned int code=0x50;
        code = (code | (rB<<12));
        rA=rA<<20;
        rB=rB<<8;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"sub"))
    {
      if(getPass()==2){
        unsigned int code=0x51;
        code = (code | (rB<<12));
        rA=rA<<20;
        rB=rB<<8;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"mul"))
    {
      if(getPass()==2){
        unsigned int code=0x52;
        code = (code | (rB<<12));
        rA=rA<<20;
        rB=rB<<8;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"div"))
    {
      if(getPass()==2){
        unsigned int code=0x53;
        code = (code | (rB<<12));
        rA=rA<<20;
        rB=rB<<8;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"and"))
    {
      if(getPass()==2){
        unsigned int code=0x61;
        code = (code | (rB<<12));
        rA=rA<<20;
        rB=rB<<8;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"or"))
    {
      if(getPass()==2){
        unsigned int code=0x62;
        code = (code | (rB<<12));
        rA=rA<<20;
        rB=rB<<8;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"xor"))
    {
      if(getPass()==2){
        unsigned int code=0x63;
        code = (code | (rB<<12));
        rA=rA<<20;
        rB=rB<<8;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"shl"))
    {
      if(getPass()==2){
        unsigned int code=0x70;
        code = (code | (rB<<12));
        rA=rA<<20;
        rB=rB<<8;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"shr"))
    {
      if(getPass()==2){
        unsigned int code=0x71;
        code = (code | (rB<<12));
        rA=rA<<20;
        rB=rB<<8;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else if (!strcmp($1, "ld"))
  {
    if(getPass()==2){
        unsigned int code=0x91;
        rA=rA<<12;
        rB=rB<<8;
        code=(code | rA | rB);
        writeCode(code);
        
      }
  }
  else{
    free($1); free($2); free($4);
    yyerror("syntax error");
    return -1;
  }}
    free($1); free($2); free($4);
    }
   | INS REG COMA REG COMA LIT{
    if(!strcmp($2,"%cause") || !strcmp($2,"%status") || !strcmp($2,"%handler") || !strcmp($4,"%cause") || !strcmp($4,"%status") || !strcmp($4,"%handler"))
    {
      free($1); free($2); free($4); free($6);
      yyerror("syntax error");
      return -1;
    }
    addLiteral($6);
    if(!strcmp($1,"beq"))
    {
      if(getPass()==2){
        unsigned int code=0xf039;
        unsigned int rB= getRegIndex($2);
        unsigned int rC= getRegIndex($4);
        int d=getLitAddr($6)-(getLoc()+4);
         if(d<-2047 || d>2047)
        {
          free($1);free($2);free($4);free($6);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL | (rB<<8)| (rC<<20));
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"bne"))
    {
      if(getPass()==2){
        unsigned int code=0xf03a;
        unsigned int rB= getRegIndex($2);
        unsigned int rC= getRegIndex($4);
        int d=getLitAddr($6)-(getLoc()+4);
         if(d<-2047 || d>2047)
        {
          free($1);free($2);free($4);free($6);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL | (rB<<8)| (rC<<20));
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"bgt"))
    {
      if(getPass()==2){
        unsigned int code=0xf03b;
        unsigned int rB= getRegIndex($2);
        unsigned int rC= getRegIndex($4);
        int d=getLitAddr($6)-(getLoc()+4);
         if(d<-2047 || d>2047)
        {
          free($1);free($2);free($4);free($6);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL | (rB<<8)| (rC<<20));
        writeCode(code);
        
      }
    }
  else{
    free($1); free($2); free($4); free($6);
    yyerror("syntax error");
    return -1;
  }
    free($1); free($2); free($4); free($6);
    }
   | INS REG COMA REG COMA OPR{
    if(!strcmp($2,"%cause") || !strcmp($2,"%status") || !strcmp($2,"%handler") || !strcmp($4,"%cause") || !strcmp($4,"%status") || !strcmp($4,"%handler"))
    {
      free($1); free($2); free($4); free($6);
      yyerror("syntax error");
      return -1;
    }
    if(!strcmp($1,"beq"))
    {
      
      if(getPass()==2){
        unsigned int code=0xf039;
        unsigned int rB= getRegIndex($2);
        unsigned int rC= getRegIndex($4);
        int d=getSymAddr($6)-(getLoc()+4);
         if(d<-2047 || d>2047)
        {
          free($1);free($2);free($4);free($6);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL | (rB<<8)| (rC<<20));
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"bne"))
    {
      if(getPass()==2){
        unsigned int code=0xf03a;
        unsigned int rB= getRegIndex($2);
        unsigned int rC= getRegIndex($4);
        int d=getSymAddr($6)-(getLoc()+4);
         if(d<-2047 || d>2047)
        {
          free($1);free($2);free($4);free($6);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL | (rB<<8)| (rC<<20));
        writeCode(code);
        
      }
    }
  else if(!strcmp($1,"bgt"))
    {
      if(getPass()==2){
        unsigned int code=0xf03b;
        unsigned int rB= getRegIndex($2);
        unsigned int rC= getRegIndex($4);
        int d=getSymAddr($6)-(getLoc()+4);
         if(d<-2047 || d>2047)
        {
          free($1);free($2);free($4);free($6);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL | (rB<<8)| (rC<<20));
        writeCode(code);
        
      }
    }
  else{
    free($1); free($2); free($4); free($6);
    yyerror("syntax error");
    return -1;
  }
    free($1); free($2); free($4); free($6);
    }
   | INS REG COMA OPR { /*ST MEMIND*/
    if(!strcmp($2,"%cause") || !strcmp($2,"%status") || !strcmp($2,"%handler"))
    {
      free($1); free($2); free($4);
      yyerror("syntax error");
      return -1;
    }
    if(!strcmp($1,"st"))
    {
      unsigned int reg=getRegIndex($2);
      unsigned int pc=getLoc()+4;
      
      if(getPass()==2)
      {
        int d=getSymAddr($4)-pc;
        unsigned int code=0xf082;
        reg=reg<<20;

        code=(code | reg);
        if(d<-2047 || d>2047)
        {
          free($1);free($2);free($4);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL);
        writeCode(code);
      }
    }
  else{
    free($1); free($2); free($4);
    yyerror("syntax error");
    return -1;
  }
    free($1); free($2); free($4);
    }
   | INS REG COMA LIT { /*ST MEMIND*/
    if(!strcmp($2,"%cause") || !strcmp($2,"%status") || !strcmp($2,"%handler"))
    {
      free($1); free($2); free($4);
      yyerror("syntax error");
      return -1;
    }
    addLiteral($4);
    if(!strcmp($1,"st"))
    {
      unsigned int reg=getRegIndex($2);
      unsigned int pc=getLoc()+4;
      
      if(getPass()==2)
      {
        int d=getLitAddr($4)-pc;
        unsigned int code=0xf082;
        reg=reg<<20;

        code=(code | reg);
        if(d<-2047 || d>2047)
        {
          free($1);free($2);free($4);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL);
        writeCode(code);
      }
    }
  else{
    free($1); free($2); free($4);
    yyerror("syntax error");
    return -1;
  }
    free($1); free($2); free($4);
    }
   | INS REG COMA LB REG RB { /*ST REGIND*/
    if(!strcmp($2,"%cause") || !strcmp($2,"%status") || !strcmp($2,"%handler") || !strcmp($5,"%cause") || !strcmp($5,"%status") || !strcmp($5,"%handler"))
    {
      free($1); free($2); free($5);
      yyerror("syntax error");
      return -1;
    }
    if(!strcmp($1,"st"))
    {
      unsigned int rA=getRegIndex($2);
      unsigned int rB=getRegIndex($5); 
      if(getPass()==2){
        unsigned int code=0x80;
        rA=rA<<20;
        rB=rB<<12;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else{
    free($1); free($2); free($5);
    yyerror("syntax error");
    return -1;
  }
    free($1); free($2); free($5);
    }
   | INS REG COMA LB REG PLUS LIT RB { /*ST REGINDPOM*/
    if(!strcmp($2,"%cause") || !strcmp($2,"%status") || !strcmp($2,"%handler") || !strcmp($5,"%cause") || !strcmp($5,"%status") || !strcmp($5,"%handler"))
    {
      free($1); free($2); free($5);free($7);
      yyerror("syntax error");
      return -1;
    }
    addLiteral($7);
    if(!strcmp($1,"st"))
    {
      if(getPass()==2){
        unsigned int code=0x80;
        int d = getNum($7);
        if(d<-2047 || d>2047)
        {
          free($1); free($2); free($5); free($7);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        unsigned int regi=getRegIndex($2);
        unsigned int regd=getRegIndex($5);
        code = (code | dh | dL|(regd<<8)|(regi<<20));
        writeCode(code);

      }
    }
  else{
    free($1); free($2); free($5); free($7);
    yyerror("syntax error");
    return -1;
  }
    free($1); free($2); free($5); free($7);
    }
   | INS DOL OPR COMA REG {/*LD IMMED*/
    if(!strcmp($5,"%cause") || !strcmp($5,"%status") || !strcmp($5,"%handler"))
    {
      free($1);free($3);free($5);
      yyerror("syntax error");
      return -1;
    }
    if(!strcmp($1,"ld"))
    {
      unsigned int reg=getRegIndex($5);
      unsigned int pc=getLoc()+4;
      

      if(getPass()==2)
      {
      int d=getSymAddr($3)-pc;
        unsigned int code=0xf00092;
        reg=reg<<12;

        code=(code | reg);
        if(d<-2047 || d>2047)
        {
          free($1);free($3);free($5);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL);
        writeCode(code);

        //writeCode(code);
      } 
      
    }
  else{
    free($1);free($3);free($5);
    yyerror("syntax error");
    return -1;
  }
    free($1);free($3);free($5);
    }
    | INS DOL LIT COMA REG {/*LD IMMED*/
    if(!strcmp($5,"%cause") || !strcmp($5,"%status") || !strcmp($5,"%handler"))
    {
      free($1);free($3);free($5);
      yyerror("syntax error");
      return -1;
    }
    addLiteral($3);
    if(!strcmp($1,"ld"))
    {
      unsigned int reg=getRegIndex($5);
      unsigned int pc=getLoc()+4;

      int d=getLitAddr($3)-pc;
      if(getPass()==2)
      {
        unsigned int code=0xf00092;
        reg=reg<<12;

        code=(code | reg);
        if(d<-2047 || d>2047)
        {
          free($1);free($3);free($5);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL);
        writeCode(code);
        //writeCode((int)(0x92|(reg)|(reg<<8)));
      }
    }
  else{
    free($1);free($3);free($5);
    yyerror("syntax error");
    return -1;
  }
    free($1);free($3);free($5);
    }
   | INS OPR COMA REG {/*LD MEMIND*/
    if(!strcmp($4,"%cause") || !strcmp($4,"%status") || !strcmp($4,"%handler"))
    {
      free($1);free($2);free($4);
      yyerror("syntax error");
      return -1;
    }
    if(!strcmp($1,"ld"))
    {
      unsigned int reg=getRegIndex($4);
      unsigned int pc=getLoc()+4;

      if(getPass()==2)
      {
      int d=getSymAddr($2)-pc;
        unsigned int code=0xf00092;
        reg=reg<<12;

        code=(code | reg);
        if(d<-2047 || d>2047)
        {
          free($1);free($2);free($4);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL);
        writeCode(code);
        writeCode((int)(0x92|(reg)|(reg<<8)));
      }
      
    }
  else{
    free($1);free($2);free($4);
    yyerror("syntax error");
    return -1;
  }
    incLoc(4);
    free($1);free($2);free($4);
    }
    | INS LIT COMA REG {/*LD MEMIND*/
    if(!strcmp($4,"%cause") || !strcmp($4,"%status") || !strcmp($4,"%handler"))
    {
      free($1);free($2);free($4);
      yyerror("syntax error");
      return -1;
    }
    addLiteral($2);
    if(!strcmp($1,"ld"))
    {
      unsigned int reg=getRegIndex($4);
      unsigned int pc=getLoc()+4;

      int d=getLitAddr($2)-pc;
      if(getPass()==2)
      {
        unsigned int code=0xf00092;
        reg=reg<<12;

        code=(code | reg);
        if(d<-2047 || d>2047)
        {
          free($1);free($2);free($4);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL);
        writeCode(code);
        writeCode((int)(0x92|(reg)|(reg<<8)));
      }    
    }
  else{
    free($1);free($2);free($4);
    yyerror("syntax error");
    return -1;
  }
    incLoc(4);
    free($1);free($2);free($4);
    }
    | INS LB REG RB COMA REG {/*LD REGIND*/
    if(!strcmp($3,"%cause") || !strcmp($3,"%status") || !strcmp($3,"%handler") ||!strcmp($6,"%cause") || !strcmp($6,"%status") || !strcmp($6,"%handler"))
    {
      free($1);free($3);free($6);
      yyerror("syntax error");
      return -1;
    }
    if(!strcmp($1,"ld"))
    {
      unsigned int rA=getRegIndex($3);
      unsigned int rB=getRegIndex($6); 
      if(getPass()==2){
        unsigned int code=0x92;
        rA=rA<<8;
        rB=rB<<12;
        code=(code | rA | rB);
        writeCode(code);
        
      }
    }
  else{
    free($1);free($3);free($6);
    yyerror("syntax error");
    return -1;
  }
    free($1);free($3);free($6);
    }
    | INS LB REG PLUS LIT RB COMA REG {/*LD REGINDPOM*/
    if(!strcmp($3,"%cause") || !strcmp($3,"%status") || !strcmp($3,"%handler") ||!strcmp($8,"%cause") || !strcmp($8,"%status") || !strcmp($8,"%handler"))
    {
      free($1);free($3);free($5);free($8);
      yyerror("syntax error");
      return -1;
    }
    addLiteral($5);
    if(!strcmp($1,"ld"))
    {
      if(getPass()==2){
        unsigned int code=0x92;
        int d = getNum($5);
        if(d<-2047 || d>2047)
        {
          free($1); free($3); free($5); free($8);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        unsigned int regi=getRegIndex($3);
        unsigned int regd=getRegIndex($8);
        code = (code | dh | dL|(regd<<12)|(regi<<20));
        writeCode(code);

      }
    }
  else{
    free($1);free($3);free($5);free($8);
    yyerror("syntax error");
    return -1;
  }
    free($1);free($3);free($5);free($8);
    }
   | INS LIT{
    
    addLiteral($2);
    if(!strcmp($1,"call"))
    {
      if(getPass()==2){
        unsigned int pc=getLoc()+4;
        unsigned int code=0xf021;
        int d=getLitAddr($2)-pc;
        if(d<-2047 || d>2047)
        {
          free($1);free($2);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL);
        writeCode(code);

      }
    }
  else if(!strcmp($1,"jmp"))
    {
      if(getPass()==2){
        unsigned int code=0xf030;
        int d=getLitAddr($2)-getLoc();
         if(d<-2047 || d>2047)
        {
          free($1);free($2);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL);
        writeCode(code);
        
      }
    }
  else{
    free($1);free($2);
    yyerror("syntax error");
    return -1;
  }
    free($1);free($2);
    }
   | INS OPR{
    
    if(!strcmp($1,"call"))
    {
      if(getPass()==2){
        unsigned int pc=getLoc()+4;
        unsigned int code=0xf021;
        int d=getSymAddr($2)-pc;
        if(d<-2047 || d>2047)
        {
          free($1);free($2);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL);
        writeCode(code);

      }
    }
  else if(!strcmp($1,"jmp"))
    {
      if(getPass()==2){
        unsigned int code=0xf038;
        int d=getSymAddr($2)-(getLoc()+4);
         if(d<-2047 || d>2047)
        {
          free($1);free($2);
          yyerror("displacement error");
          return -1;
        }
        unsigned int dh=d & 0xf00;
        unsigned int dL= d & 0xff;
        dh=dh<<8;
        dL=dL<<24;
        code = (code | dh | dL);
        writeCode(code);
        
      }
    }
  else{
    free($1);free($2);
    yyerror("syntax error");
    return -1;
  }
    free($1);free($2);
    }
   ;
dir1:
  ASM{
  if(!strcmp($1,".end"))
  {if(getPass()==1)
    set_end();
    setCur();
    free($1);
    return 0;
  }
  else{
    free($1);
    yyerror("syntax error");
    return -1;
  } 
  }
  | ASM OPR {
  if(!strcmp($1,".word")){
    if(getPass()==1)
      addSym($2,"0x0","und");
    else
    {
     writeCode(0); 
    }

    incLoc(4);
  }
  else if(!strcmp($1,".section")){
   if(getPass()==1){
      if(setCurrSec($2)==-1){
        free($1);free($2);
        yyerror("multiple section definiton error");
        return -1;
      }
    }
  }
  else if(!strcmp($1,".extern")){
    if(getPass()==1){
      if(addSym($2,"0x0","e")==-1){
        free($1);free($2);
        yyerror("multiple symbol definiton error");
        return -1;
      }
    }
  }
  else if(!strcmp($1,".global")){
    if(getPass()==1){
      if(addSym($2,"0x0","g")==-1){
        free($1);free($2);
        yyerror("multiple symbol definiton error");
        return -1;
      }
    }
  }
  else{
    free($1);free($2);
    yyerror("syntax error");
    return -1;
  }
    $$=malloc(strlen($1)+strlen($2)+2);
    strcpy($$,$1);
    strcat($$," ");
    strcat($$,$2);
    free($1);free($2);
  }
  | ASM LIT {
    if(!strcmp($1,".word")){
      unsigned int number;
        if($2[0]=='0' && $2[1]=='x')
          number=(int)strtol($2, NULL, 0);
        else
          number=atoi($2);
      incLoc(4);
      if(getPass()==2)
      {
        writeCode(number);
      } 
    }
    else if(!strcmp($1,".skip"))
    {
      unsigned int number;
      if($2[0]=='0' && $2[1]=='x')
        number=(int)strtol($2, NULL, 0);
      else
        number=atoi($2);
      
      incLoc(number);
      if(getPass()==2)
      {
        addZeros(number);
      }
    
    }
    else{
    free($1);free($2);
    yyerror("syntax error");
    return -1;
   }
    $$=malloc(strlen($1)+strlen($2)+2);
    strcpy($$,$1);
    strcat($$," ");
    strcat($$,$2);
    free($1);free($2);
  }
  ;
dir2:
  dir1 COMA LIT{
    if(!strncmp($1,".word",5)){
    unsigned int number;
      if($3[0]=='0' && $3[1]=='x')
        number=(int)strtol($3, NULL, 0);
      else
        number=atoi($3);
    incLoc(4);
    if(getPass()==2)
    {
      writeCode(number);
    }  
    
  }
  else{
    free($1);free($3);
    yyerror("syntax error");
    return -1;
  }
    $$=malloc(strlen($1)+strlen($3)+2);
    strcpy($$,$1);
    strcat($$," ");
    strcat($$,$3);
    free($1);free($3);
  }
  |dir1 COMA OPR {
  if(!strncmp($1,".word",5)){
    incLoc(4);
    if(getPass()==1)
      addSym($3,"0x0","und");
    else
    {
      writeCode(0);
    }
  }
  else if(!strncmp($1,".extern",7)){
    if(getPass()==1){
      if(addSym($3,"0x0","e")==-1){
        free($1);free($3);
        yyerror("multiple symbol definiton error");
        return -1;
      }
    }
  }
  else if(!strncmp($1,".global",7)){
    if(getPass()==1){
      if(addSym($3,"0x0","g")==-1){
        free($1);free($3);
        yyerror("multiple symbol definiton error");
        return -1;
      }
    }
  }
  else{
    free($1);free($3);
    yyerror("syntax error");
    return -1;
  }
    $$=malloc(strlen($1)+strlen($3)+2);
    strcpy($$,$1);
    strcat($$," ");
    strcat($$,$3);
    free($1);free($3);
    }
  | dir2 COMA OPR {
  if(!strncmp($1,".word",5)){
    incLoc(4);
    if(getPass()==1)
      addSym($3,"0x0","und");
    else
    {
      writeCode(0);
    }
  }
  else if(!strncmp($1,".extern",7)){
    if(getPass()==1){
      if(addSym($3,"0x0","e")==-1){
        free($1);free($3);
        yyerror("multiple symbol definiton error");
        return -1;
      }
    }
  }
  else if(!strncmp($1,".global",7)){
    if(getPass()==1){
      if(addSym($3,"0x0","g")==-1){
        free($1);free($3);
        yyerror("multiple symbol definiton error");
        return -1;
      }
    }
  }
  else{
    free($1);free($3);
    yyerror("syntax error");
    return -1;
  }
    $$=malloc(strlen($1)+strlen($3)+2);
    strcpy($$,$1);
    strcat($$," ");
    strcat($$,$3);
    free($1);free($3);
    }
  | dir2 COMA LIT {
    if(!strncmp($1,".word",5)){
    unsigned int number;
      if($3[0]=='0' && $3[1]=='x')
        number=(int)strtol($3, NULL, 0);
      else
        number=atoi($3);
    incLoc(4);
    if(getPass()==2)
    {
      writeCode(number);
    }  
  }
  else{
    free($1);free($3);
    yyerror("syntax error");
    return -1;
  }
    $$=malloc(strlen($1)+strlen($3)+2);
    strcpy($$,$1);
    strcat($$," ");
    strcat($$,$3);
    free($1);free($3);
  }
;
%%


int main(int argc,char* argv[]){
  if (argc!=4)
   return yyerror("bad arguments");
  if (strcmp(argv[1],"-o"))
   return yyerror("bad arguments");
  char* file=argv[3];
  //int size=sizeof(files)/sizeof(files[0]);
  start_asm(file,argv[2]);
  for (int i=0;i<2;i++){
    FILE* input=fopen(file,"r");
    if (input == NULL) {
        yyerror("Failed to open file");
          return -1;
    }
    yyset_in(input);
    if(yyparse()!=0)
      return -1;
    fclose(input);
    //printSymTab();
    //printLiteralPool();
   // printf("\n");
    changePass();
  }
  init_file();
  reset_asm();
  return 0;
}

yyerror(char* s) {
  printf("ERROR: %s\n", s);
  return 0;
}
