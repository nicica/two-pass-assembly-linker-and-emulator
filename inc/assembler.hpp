#ifndef _assembler_h_
#define _assembler_h_
#include "../pars.tab.h"
#include <stdlib.h>




#ifdef __cplusplus
extern "C" void start_asm(char* files,char* newFile);
extern "C" void reset_asm();
extern "C" void changePass();
extern "C" int getPass();
extern "C" void incLoc(int);
extern "C" int getLoc();
extern "C" void printSymTab();
extern "C" int addSym(char* name,char* loc,char* flag);
extern "C" void addLiteral(char* l);
extern "C" void printLiteralPool();
extern "C" void set_end();
extern "C" void writeCode(unsigned int code);
extern "C" int getRegIndex(char* reg);
extern "C" void addZeros(int num);
extern "C" unsigned int getLitAddr(char *lit);
extern "C" unsigned int getSymAddr(char* lit);
extern "C" unsigned int getSymVal(char* lit);
extern "C" void init_file();
extern "C" int getNum(char *lit);
extern "C" void setCur();
extern "C" int setCurrSec(char* name);
extern "C" int getOffs();
#else
void start_asm(char* files,char* newFile);
void reset_asm();
void changePass();
int getPass();
int getLoc();
void incLoc(int);
void printSymTab();
int addSym(char* name,char* loc,char* flag);
void addLiteral(char* l);
void printLiteralPool();
void set_end();
void writeCode(unsigned int code);
int getRegIndex(char* reg);
void addZeros(int num);
unsigned int getLitAddr(char *lit);
unsigned int getSymAddr(char* lit);
unsigned int getSymAddr(char* lit);
void init_file();
int getNum(char *lit);
void setCur();
int setCurrSec(char* name);
int getOffs();
#endif




#endif
