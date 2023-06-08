CC := gcc
CXX := g++
CFLAGS := -Wall -Wextra 
CXXFLAGS := -Wall -Wextra
LDFLAGS :=

.PHONY: all clean

all: asembler linker emulator

asembler: inc/assembler.hpp src/assembler.cpp lex.yy.c pars.tab.c
	$(CC) $(CFLAGS) $^ -lstdc++ -o $@

linker: inc/linker.hpp inc/linkerArgs.hpp src/linkerArgs.cpp src/linker.cpp src/linkerMain.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

emulator: inc/emulator.hpp src/emulator.cpp src/emulatorMain.cpp
	$(CXX) $(CXXFLAGS) $^ -g -o $@

lex.yy.c: misc/lex.l
	flex $^

pars.tab.c: misc/pars.y
	bison -t -d $^

clean:
	rm -f asembler linker emulator lex.yy.c pars.tab.c pars.tab.h
