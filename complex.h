#ifndef COMPLEX_H
#define COMPLEX_H

#include <math.h>

class Complex;

extern double pi;

class Complex
{
public:
    friend Complex operator +(const Complex c1,const Complex c2);
    friend Complex operator *(const Complex c1,const Complex c2);
    friend Complex operator -(const Complex c1,const Complex c2);
    friend Complex operator /(const Complex c1,const Complex c2);
    friend Complex exp(const Complex c);
    friend Complex sin(const Complex c);
    friend Complex cos(const Complex c);
    friend Complex tan(const Complex c);
    friend Complex pow(const Complex a,const Complex b);
    Complex();
    Complex(double fr):fR(fr),fI(0){}
    Complex(double fr,double fi):fR(fr),fI(fi){}
    void operator=(const Complex& other){fR=other.fR; fI=other.fI;}
    Complex operator -() const {return Complex(-fR,-fI);}
    double norm2() const {return fR*fR+fI*fI;}
    double norm() const {return sqrt(norm2());}
    Complex conj(){return Complex(fR,-fI);}
    Complex inv() const {return Complex(fR/norm2(),-fI/norm2());}
    inline void operator *=(Complex other){double tmp=fR; fR=fR*other.fR-fI*other.fI; fI=tmp*other.fI+fI*other.fR;}
    inline void operator /=(Complex other){*this*=other.inv();}
    inline void operator +=(Complex other){fR+=other.fR; fI+=other.fI;}
    inline void operator -=(Complex other){fR-=other.fR; fI-=other.fI;}
    double R() const {return fR;}
    double I() const {return fI;}
    void setR(double fr){fR=fr;}
    void setI(double fi){fI=fi;}
    void setC(double fr,double fi){fR=fr;fI=fi;}
    void setPrimRoot(int n){fR=cos(2*pi/n);fI=-sin(2*pi/n);}
private:
    double fR,fI;
};


#endif // COMPLEX_H
