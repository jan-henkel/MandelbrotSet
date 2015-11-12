#include "mathparser.h"

Char op[NUMOP]={'+','-','*','/','^'};
String funcName[NUMFUNC]={"sin","cos","tan","exp","pow","log","Re","Im","sqrt","abs","asin","acos","atan","sinh","cosh","tanh","asinh","acosh","atanh"};
qint32 funcNumArgs[NUMFUNC]={1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
qint32 opcode[NUMOP]={ADD,SUB,MULT,DIV,POW_N};
qint32 funcCode[NUMFUNC]={SIN,COS,TAN,EXP,POW,LOG,RE,IM,SQRT,ABS,ASIN,ACOS,ATAN,SINH,COSH,TANH,ASINH,ACOSH,ATANH};

double Re(double r)
{
    return r;
}

double Im(double)
{
    return 0;
}

std::complex<double> Re(std::complex<double> c)
{
    return (std::complex<double>)c.real();
}

std::complex<double> Im(std::complex<double> c)
{
    return (std::complex<double>)c.imag();
}
