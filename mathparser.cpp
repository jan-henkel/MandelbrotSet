#include "mathparser.h"

Char op[NUMOP]={'+','-','*','/','^'};
String funcName[NUMFUNC]={"sin","cos","tan","exp","pow","log","Re","Im","sqrt"};
int funcNumArgs[NUMFUNC]={1,1,1,1,2,1,1,1,1};
int opcode[NUMOP]={ADD,SUB,MULT,DIV,POW_N};
int funcCode[NUMFUNC]={SIN,COS,TAN,EXP,POW,LOG,RE,IM,SQRT};

double Re(double r)
{
    return r;
}

double Im(double)
{
    return 0;
}
