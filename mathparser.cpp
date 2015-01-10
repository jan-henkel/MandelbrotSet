#include "mathparser.h"

Char op[NUMOP]={'+','-','*','/','^'};
String funcName[NUMFUNC]={"sin","cos","tan","exp","pow"};
int funcNumArgs[NUMFUNC]={1,1,1,1,2};
int opcode[NUMOP]={ADD,SUB,MULT,DIV,POW_N};
int funcCode[NUMFUNC]={SIN,COS,TAN,EXP,POW};
