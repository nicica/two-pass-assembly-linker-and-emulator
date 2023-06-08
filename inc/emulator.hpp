#ifndef _emulator_h_
#define _emulator_h_

#include <iostream>
#include <string>
#include <fstream>

using namespace std;


class Emulator{
  private:
    unsigned int gpr[16];
    unsigned int csr[3];

    fstream mem;
    unsigned int current_instruction;

    void load();
    void store();
    void jump();
    void software_interrupt();
    void call();
    void xchg();
    void a_op();
    void l_op();
    void shift();
    void wrong_op_code_intterupt();
    void halt();

    bool error=false;
  public:
    Emulator(string s);

    void emulate();
};


#endif