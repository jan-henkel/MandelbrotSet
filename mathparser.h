#ifndef MATHPARSER_H
#define MATHPARSER_H
#define STACKSIZE 256
#define INSTRUCTIONLISTSIZE 256
#define NUMFUNC 19
#define NUMOP 5
#include <stdio.h>
#include <string.h>
#include <complex>
#include <cmath>
#include <QString>

typedef QString String;
typedef QChar Char;

extern Char op[NUMOP];
extern String funcName[NUMFUNC];
extern qint32 funcNumArgs[NUMFUNC];
enum Instruction{PUSHVAR,PUSHVAL,ADD,PVARADD,PVALADD,SUB,PVARSUB,PVALSUB,MULT,PVARMULT,PVALMULT,DIV,PVARDIV,PVALDIV,INV,PVARINV,PVALINV,POW_N,PVARPOW_N,PVALPOW_N,POW,PVARPOW,PVALPOW,SIN,PVARSIN,PVALSIN,COS,PVARCOS,PVALCOS,TAN,PVARTAN,PVALTAN,EXP,PVAREXP,PVALEXP,LOG,PVARLOG,PVALLOG,RE,PVARRE,PVALRE,IM,PVARIM,PVALIM,SQRT,PVARSQRT,PVALSQRT,NEG,PVARNEG,PVALNEG,ABS,PVARABS,PVALABS,ASIN,PVARASIN,PVALASIN,ACOS,PVARACOS,PVALACOS,ATAN,PVARATAN,PVALATAN,SINH,PVARSINH,PVALSINH,COSH,PVARCOSH,PVALCOSH,TANH,PVARTANH,PVALTANH,ASINH,PVARASINH,PVALASINH,ACOSH,PVARACOSH,PVALACOSH,ATANH,PVARATANH,PVALATANH};
extern qint32 opcode[NUMOP];
extern qint32 funcCode[NUMFUNC];

extern double Re(double);
extern double Im(double);
extern std::complex<double> Re(std::complex<double> c);
extern std::complex<double> Im(std::complex<double> c);

template <class C>
class MathEval
{
public:
    MathEval():stackPos(0), endInstr(instructionList), instrptr(instructionList), dataptr(data){}
    void reset(){instrptr=instructionList; stackPos=0; dataptr=data; endInstr=instructionList;}
    C* getVarPtr(char c){return (c>='a' && c<='z')?(&variables[c-'a']):0;}
    inline void pushPtrVal(C* ptr){stack[stackPos++]=*ptr;}
    inline void pushVar(qint32 c){stack[stackPos++]=*(variables+c);}
    inline void pushVal(C val){stack[stackPos++]=val;}
    C pop(){return stack[--stackPos];}
    C result(){return stack[stackPos-1];}
    void run(){
        stackPos=0;
        dataptr=data;
        instrptr=instructionList;
        while(instrptr!=endInstr) processInstruction();
    }
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
            break;
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
            break;
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
            break;
        case LOG:
            {log_();}
            break;
        case PVARLOG:
            {pvarlog_();}
            break;
        case PVALLOG:
            {pvallog_();}
            break;
        case RE:
            {re_();}
            break;
        case PVARRE:
            {pvarre_();}
            break;
        case PVALRE:
            {pvalre_();}
            break;
        case IM:
            {im_();}
            break;
        case PVARIM:
            {pvarim_();}
            break;
        case PVALIM:
            {pvalim_();}
            break;
        case SQRT:
            {sqrt_();}
            break;
        case PVARSQRT:
            {pvarsqrt_();}
            break;
        case PVALSQRT:
            {pvalsqrt_();}
            break;
        case NEG:
            {neg_();}
            break;
        case PVARNEG:
            {pvarneg_();}
            break;
        case PVALNEG:
            {pvalneg_();}
            break;
        case ABS:
            {abs_();}
            break;
        case PVARABS:
            {pvarabs_();}
            break;
        case PVALABS:
            {pvalabs_();}
            break;
        case ASIN:
            {asin_();}
            break;
        case PVARASIN:
            {pvarasin_();}
            break;
        case PVALASIN:
            {pvalasin_();}
            break;
        case ACOS:
            {acos_();}
            break;
        case PVARACOS:
            {pvaracos_();}
            break;
        case PVALACOS:
            {pvalacos_();}
            break;
        case ATAN:
            {atan_();}
            break;
        case PVARATAN:
            {pvaratan_();}
            break;
        case PVALATAN:
            {pvalatan_();}
            break;
        case SINH:
            {sinh_();}
            break;
        case PVARSINH:
            {pvarsinh_();}
            break;
        case PVALSINH:
            {pvalsinh_();}
            break;
        case COSH:
            {cosh_();}
            break;
        case PVARCOSH:
            {pvarcosh_();}
            break;
        case PVALCOSH:
            {pvalcosh_();}
            break;
        case TANH:
            {tanh_();}
            break;
        case PVARTANH:
            {pvartanh_();}
            break;
        case PVALTANH:
            {pvaltanh_();}
            break;
        case ASINH:
            {asinh_();}
            break;
        case PVARASINH:
            {pvarasinh_();}
            break;
        case PVALASINH:
            {pvalasinh_();}
            break;
        case ACOSH:
            {acosh_();}
            break;
        case PVARACOSH:
            {pvaracosh_();}
            break;
        case PVALACOSH:
            {pvalacosh_();}
            break;
        case ATANH:
            {atanh_();}
            break;
        case PVARATANH:
            {pvaratanh_();}
            break;
        case PVALATANH:
            {pvalatanh_();}
            break;
        }
    }
    inline void pushvar_() {pushVar(readInt());}
    inline void pushval_() {pushVal(readVal());}
    inline void add_(){stack[stackPos-2]+=stack[stackPos-1];--stackPos;}
    inline void pvaradd_(){stack[stackPos-1]+=readVar();}
    inline void pvaladd_(){stack[stackPos-1]+=readVal();}
    inline void sub_(){stack[stackPos-2]-=stack[stackPos-1];--stackPos;}
    inline void pvarsub_(){stack[stackPos-1]-=readVar();}
    inline void pvalsub_(){stack[stackPos-1]-=readVal();}
    inline void mult_(){stack[stackPos-2]*=stack[stackPos-1];--stackPos;}
    inline void pvarmult_(){stack[stackPos-1]*=readVar();}
    inline void pvalmult_(){stack[stackPos-1]*=readVal();}
    inline void div_(){stack[stackPos-2]/=stack[stackPos-1];--stackPos;}
    inline void pvardiv_(){stack[stackPos-1]/=readVar();}
    inline void pvaldiv_(){stack[stackPos-1]/=readVal();}
    inline void inv_(){stack[stackPos-1]=1./stack[stackPos-1];}
    inline void pvarinv_(){stack[stackPos++]=1./(readVar());}
    inline void pvalinv_(){stack[stackPos++]=1./(readVal());}
    inline void pow_n_(){stack[stackPos-1]=pow_(stack[stackPos-1],readInt());}
    inline void pvarpow_n_(){C val=readVar(); stack[stackPos++]=pow_(val,readInt());}
    inline void pvalpow_n_(){C val=readVal(); stack[stackPos++]=pow_(val,readInt());}
    inline void pow_() {stack[stackPos-2]=std::pow(stack[stackPos-2],stack[stackPos-1]);--stackPos;}
    inline void pvarpow_(){stack[stackPos-1]=std::pow(stack[stackPos-1],readVar());}
    inline void pvalpow_(){stack[stackPos-1]=std::pow(stack[stackPos-1],readVal());}
    inline void sin_() {stack[stackPos-1]=std::sin(stack[stackPos-1]);}
    inline void pvarsin_(){stack[stackPos++]=std::sin(readVar());}
    inline void pvalsin_(){stack[stackPos++]=std::sin(readVal());}
    inline void cos_() {stack[stackPos-1]=std::cos(stack[stackPos-1]);}
    inline void pvarcos_(){stack[stackPos++]=std::cos(readVar());}
    inline void pvalcos_(){stack[stackPos++]=std::cos(readVal());}
    inline void tan_() {stack[stackPos-1]=std::tan(stack[stackPos-1]);}
    inline void pvartan_(){stack[stackPos++]=std::tan(readVar());}
    inline void pvaltan_(){stack[stackPos++]=std::tan(readVal());}
    inline void exp_() {stack[stackPos-1]=std::exp(stack[stackPos-1]);}
    inline void pvarexp_(){stack[stackPos++]=std::exp(readVar());}
    inline void pvalexp_(){stack[stackPos++]=std::exp(readVal());}
    inline void log_() {stack[stackPos-1]=log(stack[stackPos-1]);}
    inline void pvarlog_(){stack[stackPos++]=log(readVar());}
    inline void pvallog_(){stack[stackPos++]=log(readVal());}
    inline void re_() {stack[stackPos-1]=Re(stack[stackPos-1]);}
    inline void pvarre_(){stack[stackPos++]=Re(readVar());}
    inline void pvalre_(){stack[stackPos++]=Re(readVal());}
    inline void im_() {stack[stackPos-1]=Im(stack[stackPos-1]);}
    inline void pvarim_(){stack[stackPos++]=Im(readVar());}
    inline void pvalim_(){stack[stackPos++]=Im(readVal());}
    inline void sqrt_() {stack[stackPos-1]=sqrt(stack[stackPos-1]);}
    inline void pvarsqrt_(){stack[stackPos++]=sqrt(readVar());}
    inline void pvalsqrt_(){stack[stackPos++]=sqrt(readVal());}
    inline void neg_() {stack[stackPos-1]=-stack[stackPos-1];}
    inline void pvarneg_() {stack[stackPos++]=-readVar();}
    inline void pvalneg_() {stack[stackPos++]=-readVal();}
    inline void abs_() {stack[stackPos-1]=std::abs(stack[stackPos-1]);}
    inline void pvarabs_() {stack[stackPos++]=std::abs(readVar());}
    inline void pvalabs_() {stack[stackPos++]=std::abs(readVal());}
    inline void asin_(){stack[stackPos-1]=std::asin(stack[stackPos-1]);}
    inline void pvarasin_() {stack[stackPos++]=std::asin(readVar());}
    inline void pvalasin_() {stack[stackPos++]=std::asin(readVal());}
    inline void acos_(){stack[stackPos-1]=std::acos(stack[stackPos-1]);}
    inline void pvaracos_() {stack[stackPos++]=std::acos(readVar());}
    inline void pvalacos_() {stack[stackPos++]=std::acos(readVal());}
    inline void atan_(){stack[stackPos-1]=std::atan(stack[stackPos-1]);}
    inline void pvaratan_() {stack[stackPos++]=std::atan(readVar());}
    inline void pvalatan_() {stack[stackPos++]=std::atan(readVal());}
    inline void sinh_(){stack[stackPos-1]=sinh(stack[stackPos-1]);}
    inline void pvarsinh_() {stack[stackPos++]=sinh(readVar());}
    inline void pvalsinh_() {stack[stackPos++]=sinh(readVal());}
    inline void cosh_(){stack[stackPos-1]=cosh(stack[stackPos-1]);}
    inline void pvarcosh_() {stack[stackPos++]=cosh(readVar());}
    inline void pvalcosh_() {stack[stackPos++]=cosh(readVal());}
    inline void tanh_(){stack[stackPos-1]=tanh(stack[stackPos-1]);}
    inline void pvartanh_() {stack[stackPos++]=tanh(readVar());}
    inline void pvaltanh_() {stack[stackPos++]=tanh(readVal());}
    inline void asinh_(){stack[stackPos-1]=asinh(stack[stackPos-1]);}
    inline void pvarasinh_() {stack[stackPos++]=asinh(readVar());}
    inline void pvalasinh_() {stack[stackPos++]=asinh(readVal());}
    inline void acosh_(){stack[stackPos-1]=acosh(stack[stackPos-1]);}
    inline void pvaracosh_() {stack[stackPos++]=acosh(readVar());}
    inline void pvalacosh_() {stack[stackPos++]=acosh(readVal());}
    inline void atanh_(){stack[stackPos-1]=atanh(stack[stackPos-1]);}
    inline void pvaratanh_() {stack[stackPos++]=atanh(readVar());}
    inline void pvalatanh_() {stack[stackPos++]=atanh(readVal());}
    inline void read(void* dst,qint32 size){memcpy(dst,dataptr,size); dataptr+=size;}
    inline void write(void* src,qint32 size){memcpy(dataptr,src,size);dataptr+=size;}
    inline C readVal(){C val=*(reinterpret_cast<C*>(dataptr)); dataptr+=sizeof(C); return val;}
    inline C* readPtr(){
        C* ptr=*(reinterpret_cast<C**>(dataptr));
        dataptr+=sizeof(C*);
        return ptr;
    }
    inline C readVar(){return variables[readInt()];}
    inline qint32 readInt(){
        qint32 i=*(reinterpret_cast<int*>(dataptr));
        dataptr+=sizeof(int);
        return i;
    }
    inline void writeVal(C val){write(&val,sizeof(C));}
    inline void writePtr(C* ptr){write(&ptr,sizeof(C*));}
    inline void writeInt(qint32 i){write(&i,sizeof(int));}
    void writeInstr(qint32 instruction)
    {
        if(*(instrptr-1)<=1 && instruction>1)
            *(instrptr-1)=instruction+*(instrptr-1)+1;
        else
            *(instrptr++)=instruction;
    }
    void setInstrEnd(){endInstr=instrptr;}

    C pow_(C val, qint32 n){return n>0?pow1(val,n):n<0?1./pow1(val,-n):1;}
    C pow1(C val,qint32 n){
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
    qint32 instructionList[INSTRUCTIONLISTSIZE];
    char data[INSTRUCTIONLISTSIZE*8];
    qint32 stackPos;
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
        qint32 lenread=parseToMathEval(0,0);
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
    qint32 parseToMathEval(qint32 level, qint32 pos)
    {
        qint32 newpos=pos;
        qint32 lenread;
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
                        qint32 n;
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
    qint32 parseAtomicExpression(qint32 pos)
    {
        bool negative=(str[pos]=='-');
        //qint32 lenread;
        qint32 newpos=pos;
        if(negative)
        {
            ++newpos;
            if(newpos>=str.length())
                return 0;
            if(str[newpos]>='0' && str[newpos]<='9')
                return parseFloatNumber(pos);
            else
            {
                newpos+=parseToMathEval(2,newpos);
                if(newpos==pos+1)
                    return 0;
                else
                {
                    mathEval->writeInstr(NEG);
                    return newpos-pos;
                }
            }
        }
        else
        {
            if(str[newpos]>='0' && str[newpos]<='9')
                return parseFloatNumber(pos);
            else if(str[pos].toLower()>='a' && str[pos].toLower()<='z')
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
                ++newpos;
                newpos+=parseToMathEval(0,newpos);
                if(newpos>=str.length())
                    return 0;
                if(str[newpos]!=')')
                    return 0;
                ++newpos;
                return newpos-pos;
            }
        }
        return 0;
    }
    qint32 readIntNumber(qint32 pos, qint32 *rNum)
    {
        qint32 newpos=pos;
        if(str[pos]=='-')
            ++newpos;
        while(newpos<str.length() && str[newpos]>='0' && str[newpos]<='9')
            ++newpos;
        if(newpos>str.length())
            return 0;
        if(str[pos]=='-' && newpos==pos+1)
            return 0;
        if(newpos<str.length() && str[newpos]=='.')
        {
            return 0;
        }
        *rNum=str.mid(pos,newpos-pos).toInt();
        return newpos-pos;
    }
    qint32 parseFloatNumber(qint32 pos)
    {
        qint32 newpos=pos;
        if(str[pos]=='-')
            ++newpos;
        while(newpos<str.length() && str[newpos]>='0' && str[newpos]<='9')
            ++newpos;
        if(newpos<str.length() && str[newpos]=='.')
        {
            ++newpos;
            while(newpos<str.length() && str[newpos]>='0' && str[newpos]<='9')
                ++newpos;
        }
        if(newpos<str.length() && str[newpos].toLower()=='e')
        {
            ++newpos;
            if(newpos<str.length() && str[newpos]=='-')
                ++newpos;
            while(newpos<str.length() && str[newpos]>='0' && str[newpos]<='9')
                ++newpos;
            if(str[newpos-1].toLower()=='e' || str[newpos-1]=='-')
                return 0;
        }
        C val=str.mid(pos,newpos-pos).toDouble();
        mathEval->writeInstr(PUSHVAL);
        mathEval->writeVal(val);
        return newpos-pos;
    }
    qint32 parseVar(qint32 pos)
    {
        char c=str[pos].toLower().toLatin1();
        mathEval->writeInstr(PUSHVAR);
        mathEval->writeInt((int)(c-'a'));
        return 1;
    }
    qint32 parseFunc(qint32 pos)
    {
        qint32 newpos=pos;
        while(str[newpos].toLower()>='a' && str[newpos].toLower()<='z')
            ++newpos;
        if(str[newpos]!='(')
            return 0;
        String funcName_=str.mid(pos,newpos-pos);
        ++newpos;
        qint32 funcIndex=0;
        while(funcIndex<NUMFUNC)
        {
            if(funcName_==funcName[funcIndex])
            {
                qint32 numArgs=funcNumArgs[funcIndex];
                qint32 lenread=parseToMathEval(0,newpos);
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
