#ifndef MATHPARSER_H
#define MATHPARSER_H
#define STACKSIZE 4096
#define INSTRUCTIONLISTSIZE 16384
#define NUMFUNC 5
#define NUMOP 5
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <QString>

typedef QString String;
typedef QChar Char;

extern Char op[NUMOP];
extern String funcName[NUMFUNC];
extern int funcNumArgs[NUMFUNC];
enum Instruction{PUSHVAR,PUSHVAL,ADD,SUB,MULT,DIV,INV,POW_N,POW,SIN,COS,TAN,EXP,CALL,END};
extern long opcode[NUMOP];
extern long funcCode[NUMFUNC];

template <class C>
class MathEval
{
public:
    MathEval():readPos(0),writePos(0),stackPos(0), done(false){}
    void reset(){readPos=0; writePos=0; stackPos=0; done=false;}
    void setVar(char c,C val){variables[c]=val;}
    C getVar(char c){return variables[c];}
    void pushVar(int c){stack[stackPos++]=variables[c];}
    void pushVal(C val){stack[stackPos++]=val;}
    C pop(){return stack[--stackPos];}
    C result(){return stack[stackPos-1];}
    void run(){done=0; stackPos=0; readPos=0; while(!done) processInstruction();}
    void processInstruction()
    {
        long instruction;
        read(&instruction,sizeof(long));
        switch(instruction)
        {
        case PUSHVAR:
            {pushvar_();}
            break;
        case PUSHVAL:
            {pushval_();}
            break;
        case ADD:
            {add_();}
            break;
        case SUB:
            {sub_();}
            break;
        case MULT:
            {mult_();}
            break;
        case DIV:
            {div_();}
            break;
        case INV:
            {inv();}
            break;
        case POW_N:
            {int n; read(&n,sizeof(int)); pow_(n);}
            break;
        case POW:
            {pow_();}
            break;
        case SIN:
            {sin_();}
            break;
        case COS:
            {cos_();}
            break;
        case TAN:
            {tan_();}
            break;
        case EXP:
            {exp_();}
            break;
        case END:
            {done=true;}
            break;
        }
    }
    inline void pushval_() {C val; read(&val,sizeof(C)); pushVal(val);}
    inline void pushvar_() {int c; read(&c,sizeof(int)); pushVar(c);}
    inline void add_(){stack[stackPos-2]+=stack[stackPos-1];--stackPos;}
    inline void sub_(){stack[stackPos-2]-=stack[stackPos-1];--stackPos;}
    inline void mult_(){stack[stackPos-2]*=stack[stackPos-1];--stackPos;}
    inline void div_(){stack[stackPos-2]/=stack[stackPos-1];--stackPos;}
    virtual void inv(){pushVal(1./pop());}
    void pow_(int n){pushVal(pow_(pop(),n));}
    void read(void* dst,int size){memcpy(dst,instructionList+readPos,size); readPos+=size;}
    void write(void* src,int size){memcpy(instructionList+writePos,src,size);writePos+=size;}
    C pow_(C val, int n){return n>0?pow1(val,n):n<0?1/pow1(val,-n):1;}
    C pow1(C val,int n){
        if(n==1)
            return val;
        else
        {
            C tmp=pow1(val,n/2);
            tmp*=tmp;
            tmp*=(n%2?val:1);
            return tmp;
        }
    }
    virtual void pow_() {C b=pop(),a=pop();
                       pushVal(pow(a,b));}
    virtual void sin_() {pushVal(sin(pop()));}
    virtual void cos_() {pushVal(cos(pop()));}
    virtual void tan_() {pushVal(tan(pop()));}
    virtual void exp_() {pushVal(exp(pop()));}

private:
    C stack[STACKSIZE];
    C variables[256];
    char instructionList[INSTRUCTIONLISTSIZE];
    int readPos;
    int writePos;
    int stackPos;
    bool done;
};

template <class C>
class MathParser
{
public:
    MathParser(){}
    MathParser<C>(MathEval<C>* me):mathEval(me){}
    void setString(String s){str=s;}
    void setMathEval(MathEval<C>* me){mathEval=me;}
    bool parse(){
        mathEval->reset();
        int lenread=parseToMathEval(0,0);
        if(lenread!=str.length())
        {
            long opcode_=END;
            mathEval->reset();
            mathEval->write(&opcode_,sizeof(long));
            return false;
        }
        else
        {
            long opcode_=END;
            mathEval->write(&opcode_,sizeof(long));
            return true;
        }
    }
private:
    int parseToMathEval(int level, int pos)
    {
        int newpos=pos;
        int lenread;
        if(level==5)
        {
            lenread=parseAtomicExpression(newpos);
            newpos+=lenread;
        }
        else
        {
            lenread=parseToMathEval(level+1,newpos);
            newpos+=lenread;
            if(level<4)
            {
                if(lenread==0)
                    return 0;
                while(lenread!=0 && newpos<str.length() && str[newpos]==op[level])
                {
                    ++newpos;
                    lenread=parseToMathEval(level+1,newpos);
                    if(lenread==0)
                    {
                        --newpos;
                        return newpos-pos;
                    }
                    mathEval->write(&opcode[level],sizeof(long));
                    newpos+=lenread;
                }
            }
            else if(level==4)
            {
                if(lenread==0)
                    return 0;
                else
                {
                    while(lenread!=0 && newpos<str.length() && str[newpos]==op[level])
                    {
                        ++newpos;
                        int n;
                        lenread=readIntNumber(newpos,&n);
                        if(lenread==0)
                        {
                            //to do: allow for atomic expressions to be read. requires further development of MathEval.
                            --newpos;
                            return newpos-pos;
                        }
                        else
                        {
                            newpos+=lenread;
                            mathEval->write(&opcode[level],sizeof(long));
                            mathEval->write(&n,sizeof(int));
                        }
                    }
                }
            }
        }
        return newpos-pos;
    }
    int parseAtomicExpression(int pos)
    {
        if((str[pos]>='0' && str[pos]<='9')||str[pos]=='-')
            return parseFloatNumber(pos);
        else if(str[pos]>='a' && str[pos]<='z')
        {
            if(pos<str.length()-1 && str[pos+1]>='a' && str[pos+1]<='z')
                return parseFunc(pos);
            else
                return parseVar(pos);
        }
        else if(str[pos]=='(')
        {
            if(pos>=str.length()-1)
                return 0;
            int newpos=pos+1;
            int lenread=parseToMathEval(0,newpos);
            newpos+=lenread;
            if(newpos>=str.length())
                return 0;
            if(str[newpos]!=')')
                return 0;
            ++newpos;
            return newpos-pos;
        }
        return 0;
    }
    int readIntNumber(int pos, int *rNum)
    {
        int newpos=pos;
        if(str[pos]=='-')
            ++newpos;
        while(str[newpos]>='0' && str[newpos]<='9')
            ++newpos;
        if(str[newpos]=='.')
        {
            return 0;
        }
        *rNum=str.mid(pos,newpos-pos).toInt();
        return newpos-pos;
    }
    int parseFloatNumber(int pos)
    {
        int newpos=pos;
        if(str[pos]=='-')
            ++newpos;
        while(str[newpos]>='0' && str[newpos]<='9')
            ++newpos;
        if(str[newpos]=='.')
        {
            ++newpos;
            while(str[newpos]>='0' && str[newpos]<='9')
                ++newpos;
        }
        C val=str.mid(pos,newpos-pos).toDouble();
        long opcode_=PUSHVAL;
        mathEval->write(&opcode_,sizeof(long));
        mathEval->write(&val,sizeof(C));
        return newpos-pos;
    }
    int parseVar(int pos)
    {
        int c=str[pos].toLatin1();
        long opcode_=PUSHVAR;
        mathEval->write(&opcode_,sizeof(long));
        mathEval->write(&c,sizeof(int));
        return 1;
    }
    int parseFunc(int pos)
    {
        int newpos=pos;
        while(str[newpos]>='a' && str[newpos]<='z')
            ++newpos;
        if(str[newpos]!='(')
            return 0;
        String funcName_=str.mid(pos,newpos-pos);
        ++newpos;
        int funcIndex=0;
        while(funcIndex<NUMFUNC)
        {
            if(funcName_==funcName[funcIndex])
            {
                int numArgs=funcNumArgs[funcIndex];
                int lenread=parseToMathEval(0,newpos);
                if(lenread==0)
                    return 0;
                newpos+=lenread;
                --numArgs;
                while(numArgs)
                {
                    if(str[newpos]!=',')
                        return 0;
                    ++newpos;
                    lenread=parseToMathEval(0,newpos);
                    if(lenread==0)
                        return 0;
                    newpos+=lenread;
                    --numArgs;
                }
                if(str[newpos]!=')')
                    return 0;
                ++newpos;
                long opcode_=funcCode[funcIndex];
                mathEval->write(&opcode_,sizeof(long));
                return newpos-pos;
            }
            ++funcIndex;
        }
        return 0;
    }

    MathEval<C> *mathEval;
    String str;
};



#endif // MATHPARSER_H
