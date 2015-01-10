#include "complex.h"

double pi=3.14159265358979323846264338327950288419716939937510;

Complex::Complex()
{
}

Complex operator *(const Complex c1,const Complex c2)
{
    return Complex(c1.fR*c2.fR-c1.fI*c2.fI,c1.fR*c2.fI+c1.fI*c2.fR);
}

Complex operator +(const Complex c1,const Complex c2)
{
    return Complex(c1.fR+c2.fR,c1.fI+c2.fI);
}

Complex operator -(const Complex c1,const Complex c2)
{
    Complex r=c1;
    r-=c2;
    return r;
}

Complex operator /(const Complex c1,const Complex c2)
{
    Complex r=c1;
    r*=c2.inv();
    return r;
}

Complex exp(const Complex c)
{
    double r=exp(c.fR);
    return Complex(r*cos(c.fI),r*sin(c.fI));
}

Complex pow(const Complex a, const Complex b)
{
    if(a.R()==0 && a.I()==0)
        return (b.R()==0 && b.I()==0)?Complex(1,0):Complex(0,0);
    else
    {
        double r=a.norm();
        double phi=(a.I()<0?-1:1)*acos(a.R()/r);
        double q=pow(r,b.R())*exp(-phi*b.I());
        return Complex(q*cos(phi*b.R()+log(r)*b.I()),q*sin(phi*b.R()+log(r)*b.I()));
    }
}

Complex cos(const Complex c)
{
    double emb=exp(-c.I()), eb=exp(c.I());
    return Complex(0.5*(emb+eb)*cos(c.R()),0.5*(emb-eb)*sin(c.R()));
}

Complex sin(const Complex c)
{
    double emb=exp(-c.I()), eb=exp(c.I());
    return Complex(0.5*(eb+emb)*sin(c.R()),0.5*(eb-emb)*cos(c.R()));
}

Complex tan(const Complex c)
{
    return sin(c)/cos(c);
}
