#include "mathparser.h"

Char op[NUMOP]={'+','-','*','/','^'};
String funcName[NUMFUNC]={"sin","cos","tan","exp","pow"};
int funcNumArgs[NUMFUNC]={1,1,1,1,2};
long opcode[NUMOP]={ADD,SUB,MULT,DIV,POW_N};
long funcCode[NUMFUNC]={SIN,COS,TAN,EXP,POW};
