#ifndef MATHPARSER_H
#define MATHPARSER_H
#define STACKSIZE 4096
#define INSTRUCTIONLISTSIZE 4096
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
enum Instruction{PUSHVAR,PUSHVAL,ADD,PVARADD,PVALADD,SUB,PVARSUB,PVALSUB,MULT,PVARMULT,PVALMULT,DIV,PVARDIV,PVALDIV,INV,PVARINV,PVALINV,POW_N,PVARPOW_N,PVALPOW_N,POW,PVARPOW,PVALPOW,SIN,PVARSIN,PVALSIN,COS,PVARCOS,PVALCOS,TAN,PVARTAN,PVALTAN,EXP,PVAREXP,PVALEXP};
extern int opcode[NUMOP];
extern int funcCode[NUMFUNC];

template <class C>
class MathEval
{
public:
    MathEval():stackPos(0), endInstr(instructionList), instrptr(instructionList), dataptr(data){}
    void reset(){instrptr=instructionList; stackPos=0; dataptr=data; endInstr=instructionList;}
    C* getVarPtr(char c){return (c>='a' && c<='z')?(&variables[c-'a']):0;}
    inline void pushPtrVal(C* ptr){stack[stackPos++]=*ptr;}
    inline void pushVal(C val){stack[stackPos++]=val;}
    C pop(){return stack[--stackPos];}
    C result(){return stack[stackPos-1];}
    void run(){stackPos=0; dataptr=data; instrptr=instructionList; while(instrptr!=endInstr) processInstruction();}
    void processInstruction()
    {
        switch(*(instrptr++))
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
        case PVARADD:
            {pvaradd_();}
            break;
        case PVALADD:
            {pvaladd_();}
            break;
        case SUB:
            {sub_();}
            break;
        case PVARSUB:
            {pvarsub_();}
            break;
        case PVALSUB:
            {pvalsub_();}
            break;
        case MULT:
            {mult_();}
            break;
        case PVARMULT:
            {pvarmult_();}
            break;
        case PVALMULT:
            {pvalmult_();}
            break;
        case DIV:
            {div_();}
            break;
        case PVARDIV:
            {pvardiv_();}
            break;
        case PVALDIV:
            {pvaldiv_();}
            break;
        case INV:
            {inv_();}
            break;
        case PVARINV:
            {pvarinv_();}
            break;
        case PVALINV:
            {pvalinv_();}
            break;
        case POW_N:
            {pow_n_();}
            break;
        case PVARPOW_N:
            {pvarpow_n_();}
            break;
        case PVALPOW_N:
            {pvalpow_n_();}
            break;
        case POW:
            {pow_();}
            break;
        case PVARPOW:
            {pvarpow_();}
            break;
        case PVALPOW:
            {pvalpow_();}
        case SIN:
            {sin_();}
            break;
        case PVARSIN:
            {pvarsin_();}
            break;
        case PVALSIN:
            {pvalsin_();}
        case COS:
            {cos_();}
            break;
        case PVARCOS:
            {pvarcos_();}
            break;
        case PVALCOS:
            {pvalcos_();}
        case TAN:
            {tan_();}
            break;
        case PVARTAN:
            {pvartan_();}
            break;
        case PVALTAN:
            {pvaltan_();}
        case EXP:
            {exp_();}
            break;
        case PVAREXP:
            {pvarexp_();}
            break;
        case PVALEXP:
            {pvalexp_();}
        }
    }
    inline void pushvar_() {pushPtrVal(readPtr());}
    inline void pushval_() {pushVal(readVal());}
    inline void add_(){stack[stackPos-2]+=stack[stackPos-1];--stackPos;}
    inline void pvaradd_(){stack[stackPos-1]+=*readPtr();}
    inline void pvaladd_(){stack[stackPos-1]+=readVal();}
    inline void sub_(){stack[stackPos-2]-=stack[stackPos-1];--stackPos;}
    inline void pvarsub_(){stack[stackPos-1]-=*readPtr();}
    inline void pvalsub_(){stack[stackPos-1]-=readVal();}
    inline void mult_(){stack[stackPos-2]*=stack[stackPos-1];--stackPos;}
    inline void pvarmult_(){stack[stackPos-1]*=*readPtr();}
    inline void pvalmult_(){stack[stackPos-1]*=readVal();}
    inline void div_(){stack[stackPos-2]/=stack[stackPos-1];--stackPos;}
    inline void pvardiv_(){stack[stackPos-1]/=*readPtr();}
    inline void pvaldiv_(){stack[stackPos-1]/=readVal();}
    inline virtual void inv_(){stack[stackPos-1]=1/stack[stackPos-1];}
    inline virtual void pvarinv_(){stack[stackPos++]=1/(*readPtr());}
    inline virtual void pvalinv_(){stack[stackPos++]=1/(readVal());}
    inline virtual void pow_n_(){stack[stackPos-1]=pow_(stack[stackPos-1],readInt());}
    inline virtual void pvarpow_n_(){C val=*readPtr(); stack[stackPos++]=pow_(val,readInt());}
    inline virtual void pvalpow_n_(){C val=readVal(); stack[stackPos++]=pow_(val,readInt());}
    inline virtual void pow_() {stack[stackPos-2]=pow(stack[stackPos-2],stack[stackPos-1]);--stackPos;}
    inline virtual void pvarpow_(){stack[stackPos-1]=pow(stack[stackPos-1],*readPtr());}
    inline virtual void pvalpow_(){stack[stackPos-1]=pow(stack[stackPos-1],readVal());}
    inline virtual void sin_() {stack[stackPos-1]=sin(stack[stackPos-1]);}
    inline virtual void pvarsin_(){stack[stackPos++]=sin(*readPtr());}
    inline virtual void pvalsin_(){stack[stackPos++]=sin(readVal());}
    inline virtual void cos_() {stack[stackPos-1]=cos(stack[stackPos-1]);}
    inline virtual void pvarcos_(){stack[stackPos++]=cos(*readPtr());}
    inline virtual void pvalcos_(){stack[stackPos++]=cos(readVal());}
    inline virtual void tan_() {stack[stackPos-1]=tan(stack[stackPos-1]);}
    inline virtual void pvartan_(){stack[stackPos++]=tan(*readPtr());}
    inline virtual void pvaltan_(){stack[stackPos++]=tan(readVal());}
    inline virtual void exp_() {stack[stackPos-1]=exp(stack[stackPos-1]);}
    inline virtual void pvarexp_(){stack[stackPos++]=exp(*readPtr());}
    inline virtual void pvalexp_(){stack[stackPos++]=exp(readVal());}
    inline void read(void* dst,int size){memcpy(dst,dataptr,size); dataptr+=size;}
    inline void write(void* src,int size){memcpy(dataptr,src,size);dataptr+=size;}
    inline C readVal(){C val=*(reinterpret_cast<C*>(dataptr)); dataptr+=sizeof(C); return val;}
    inline C* readPtr(){
        C* ptr=*(reinterpret_cast<C**>(dataptr));
        dataptr+=sizeof(C*);
        return ptr;
    }
    inline int readInt(){
        int i=*(reinterpret_cast<int*>(dataptr));
        dataptr+=sizeof(int);
        return i;
    }
    inline void writeVal(C val){write(&val,sizeof(C));}
    inline void writePtr(C* ptr){write(&ptr,sizeof(C*));}
    inline void writeInt(int i){write(&i,sizeof(int));}
    void writeInstr(int instruction)
    {
        if(*(instrptr-1)<=1 && instruction>1)
            *(instrptr-1)=instruction+*(instrptr-1)+1;
        else
            *(instrptr++)=instruction;
    }
    void setInstrEnd(){endInstr=instrptr;}

    C pow_(C val, int n){return n>0?pow1(val,n):n<0?1/pow1(val,-n):1;}
    C pow1(C val,int n){
        if(n==1)
            return val;
        else
        {
            C tmp=pow1(val,n/2);
            tmp*=tmp;
            if(n%2)
                tmp*=val;
            return tmp;
        }
    }

private:
    C stack[STACKSIZE];
    C variables[26];
    int instructionList[INSTRUCTIONLISTSIZE];
    char data[INSTRUCTIONLISTSIZE*8];
    int stackPos;
    int* endInstr;
    int* instrptr;
    char* dataptr;
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
            mathEval->reset();
            return false;
        }
        else
        {
            mathEval->setInstrEnd();
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
                    mathEval->writeInstr(opcode[level]);
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
                            --newpos;
                            return newpos-pos;
                        }
                        else
                        {
                            newpos+=lenread;
                            mathEval->writeInstr(opcode[level]);
                            mathEval->writeInt(n);
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
        mathEval->writeInstr(PUSHVAL);
        mathEval->writeVal(val);
        return newpos-pos;
    }
    int parseVar(int pos)
    {
        int c=str[pos].toLatin1();
        mathEval->writeInstr(PUSHVAR);
        mathEval->writePtr(mathEval->getVarPtr(c));
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
                mathEval->writeInstr(funcCode[funcIndex]);
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
