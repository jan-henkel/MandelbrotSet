#include "mandelbrotset.h"
#include <QMessageBox>
#include <QColor>

QRgb colorInterp(QColor col[4],double x,double y)
{
    return qRgb(
            (int)(((double)col[0].red()*(1.-x)+(double)col[1].red()*x)*(1.-y)+((double)col[2].red()*(1.-x)+(double)col[3].red()*x)*y),
            (int)(((double)col[0].green()*(1.-x)+(double)col[1].green()*x)*(1.-y)+((double)col[2].green()*(1.-x)+(double)col[3].green()*x)*y),
            (int)(((double)col[0].blue()*(1.-x)+(double)col[1].blue()*x)*(1.-y)+((double)col[2].blue()*(1.-x)+(double)col[3].blue()*x)*y)
            );
}

void MandelbrotSet::renderMandelbrot(double xCenter, double yCenter, int width, int height, double scale, int nIterations, double limit, int nPasses)
{
    emit errorCodeOut(errorCode_);
    if(errorCode_)
        return;
    int halfWidth=width/2;
    int halfHeight=height/2;
    QImage image(width,height,QImage::Format_RGB32);
    int paletteWidth=colorPalette_.width();
    int paletteHeight=colorPalette_.height();
    unsigned long *palette=reinterpret_cast<unsigned long*>(colorPalette_.scanLine(0));
    //z=z(n), c=Complex(x,y) with coordinates x and y on the complex plane corresponding to points of the image
    Complex *ec=eval_.getVarPtr('c'),*ez=eval_.getVarPtr('z');
    //imaginary unit i
    (*eval_.getVarPtr('i'))=Complex(0.0,1.0);
    //s=Re(z), t=Im(z) when iteration loop is done
    double *e1s=paletteXeval_.getVarPtr('s'), *e1t=paletteXeval_.getVarPtr('t');
    double *e2s=paletteYeval_.getVarPtr('s'), *e2t=paletteYeval_.getVarPtr('t');
    //u=Re(c), v=Im(c)
    double *e1u=paletteXeval_.getVarPtr('u'), *e1v=paletteXeval_.getVarPtr('v');
    double *e2u=paletteYeval_.getVarPtr('u'), *e2v=paletteYeval_.getVarPtr('v');
    //n=it (number of iterations before escape), m=nIterations (max. number of iterations), l=limit
    double *e1n=paletteXeval_.getVarPtr('n'), *e1m=paletteXeval_.getVarPtr('m'), *e1l=paletteXeval_.getVarPtr('l');
    double *e2n=paletteYeval_.getVarPtr('n'), *e2m=paletteYeval_.getVarPtr('m'), *e2l=paletteXeval_.getVarPtr('l');
    //w=paletteWidth, h=paletteHeight
    double *e1w=paletteXeval_.getVarPtr('w'), *e1h=paletteXeval_.getVarPtr('h');
    double *e2w=paletteYeval_.getVarPtr('w'), *e2h=paletteYeval_.getVarPtr('h');

    *e1m=*e2m=(double)nIterations;
    *e1l=*e2l=limit;
    *e1w=*e2w=(double)paletteWidth;
    *e1h=*e2h=(double)paletteHeight;
    col0Interior_&=(paletteWidth>1);
    row0Interior_&=(paletteHeight>1);
    const int upperLimit=(1<<(sizeof(int)*8-2));
    for(int pass=0;pass<nPasses;++pass)
    {
        int nIt=nIterations>>(2*(nPasses-pass-1));
        for(int iy=0;iy<height;++iy)
        {
            unsigned long *scanline=reinterpret_cast<unsigned long*>(image.scanLine(iy));
            for(int ix=0;ix<width;++ix)
            {
                double x=(ix-halfWidth)*scale+xCenter;
                double y=(iy-halfHeight)*scale+yCenter;

                *ec=Complex(x,y);
                *ez=Complex(0,0);
                int it=0;
                while(it<nIt && ez->norm2()<=limit)
                {
                    eval_.run();
                    *ez=eval_.result();
                    ++it;
                }
                *e1s=*e2s=ez->R();
                *e1t=*e2t=ez->I();
                *e1u=*e2u=ec->R();
                *e1v=*e2v=ec->I();
                *e1n=*e2n=(double)it;
                double xPal,yPal;
                int ixPal,iyPal;
                paletteXeval_.run();
                paletteYeval_.run();
                xPal=paletteXeval_.result();
                yPal=paletteYeval_.result();
                xPal=(xPal<0 || xPal>upperLimit)?0:xPal;
                yPal=(yPal<0 || yPal>upperLimit)?0:yPal;
                ixPal=(int)xPal;
                iyPal=(int)yPal;
                int index[4];
                QColor col[4];
                if(it==nIt)
                {
                    if(col0Interior_)
                    {
                        xPal=0;
                        ixPal=0;
                    }
                    if(row0Interior_)
                    {
                        yPal=0;
                        iyPal=0;
                    }
                    index[0]=ixPal%paletteWidth+(iyPal%paletteHeight)*paletteWidth;
                    index[1]=(ixPal+1)%paletteWidth+(iyPal%paletteHeight)*paletteWidth;
                    index[2]=ixPal%paletteWidth+((iyPal+1)%paletteHeight)*paletteWidth;
                    index[3]=(ixPal+1)%paletteWidth+((iyPal+1)%paletteHeight)*paletteWidth;
                }
                else
                {
                    index[0]=(int)col0Interior_+(int)row0Interior_*paletteWidth+ixPal%(paletteWidth-(int)col0Interior_)+(iyPal%(paletteHeight-(int)row0Interior_))*paletteWidth;
                    index[1]=(int)col0Interior_+(int)row0Interior_*paletteWidth+(ixPal+1)%(paletteWidth-(int)col0Interior_)+(iyPal%(paletteHeight-(int)row0Interior_))*paletteWidth;
                    index[2]=(int)col0Interior_+(int)row0Interior_*paletteWidth+ixPal%(paletteWidth-(int)col0Interior_)+((iyPal+1)%(paletteHeight-(int)row0Interior_))*paletteWidth;
                    index[3]=(int)col0Interior_+(int)row0Interior_*paletteWidth+(ixPal+1)%(paletteWidth-(int)col0Interior_)+((iyPal+1)%(paletteHeight-(int)row0Interior_))*paletteWidth;
                }
                for(int i=0;i<4;++i)
                    col[i]=QColor(palette[index[i]]);
                scanline[ix]=colorInterp(col,xPal-ixPal,yPal-iyPal);
            }
        }
        emit imageOut(image);
    }
}

void MandelbrotSet::renderJulia(double xCenter, double yCenter, int width, int height, double scale, int nIterations, double limit, int nPasses, double cRe, double cIm)
{
    emit errorCodeOut(errorCode_);
    if(errorCode_)
        return;
    int halfWidth=width/2;
    int halfHeight=height/2;
    QImage image(width,height,QImage::Format_RGB32);
    int paletteWidth=colorPalette_.width();
    int paletteHeight=colorPalette_.height();
    unsigned long *palette=reinterpret_cast<unsigned long*>(colorPalette_.scanLine(0));
    Complex *ec=eval_.getVarPtr('c'),*ez=eval_.getVarPtr('z');
    //imaginary unit i
    (*eval_.getVarPtr('i'))=Complex(0.0,1.0);
    //s=Re(z), t=Im(z) when iteration loop is done
    double *e1s=paletteXeval_.getVarPtr('s'), *e1t=paletteXeval_.getVarPtr('t');
    double *e2s=paletteYeval_.getVarPtr('s'), *e2t=paletteYeval_.getVarPtr('t');
    //u=x, v=y
    double *e1u=paletteXeval_.getVarPtr('u'), *e1v=paletteXeval_.getVarPtr('v');
    double *e2u=paletteYeval_.getVarPtr('u'), *e2v=paletteYeval_.getVarPtr('v');
    //n=it (number of iterations before escape), m=nIterations (max. number of iterations), l=limit
    double *e1n=paletteXeval_.getVarPtr('n'), *e1m=paletteXeval_.getVarPtr('m'), *e1l=paletteXeval_.getVarPtr('l');
    double *e2n=paletteYeval_.getVarPtr('n'), *e2m=paletteYeval_.getVarPtr('m'), *e2l=paletteXeval_.getVarPtr('l');
    //w=paletteWidth, h=paletteHeight
    double *e1w=paletteXeval_.getVarPtr('w'), *e1h=paletteXeval_.getVarPtr('h');
    double *e2w=paletteYeval_.getVarPtr('w'), *e2h=paletteYeval_.getVarPtr('h');

    const int upperLimit=(1<<(sizeof(int)*8-2));
    *e1m=*e2m=nIterations;
    *e1l=*e2l=limit;
    *e1w=*e2w=(double)paletteWidth;
    *e1h=*e2h=(double)paletteHeight;
    *ec=Complex(cRe,cIm);
    col0Interior_&=(paletteWidth>1);
    row0Interior_&=(paletteHeight>1);
    for(int pass=0;pass<nPasses;++pass)
    {
        int nIt=nIterations>>(2*(nPasses-pass));
        for(int iy=0;iy<height;++iy)
        {
            unsigned long *scanline=reinterpret_cast<unsigned long*>(image.scanLine(iy));
            for(int ix=0;ix<width;++ix)
            {
                double x=(ix-halfWidth)*scale+xCenter;
                double y=(iy-halfHeight)*scale+yCenter;

                *ez=Complex(x,y);
                int it=0;
                while(it<nIt && ez->norm2()<=limit)
                {
                    eval_.run();
                    *ez=eval_.result();
                    ++it;
                }
                *e1s=*e2s=ez->R();
                *e1t=*e2t=ez->I();
                *e1u=*e2u=x;
                *e1v=*e2v=y;
                *e1n=*e2n=it;
                double xPal,yPal;
                int ixPal,iyPal;
                paletteXeval_.run();
                paletteYeval_.run();
                xPal=paletteXeval_.result();
                yPal=paletteYeval_.result();
                xPal=(xPal<0 || xPal>upperLimit)?0:xPal;
                yPal=(yPal<0 || yPal>upperLimit)?0:yPal;
                ixPal=(int)xPal;
                iyPal=(int)yPal;
                int index[4];
                QColor col[4];
                if(it==nIt)
                {
                    if(col0Interior_)
                    {
                        xPal=0;
                        ixPal=0;
                    }
                    if(row0Interior_)
                    {
                        yPal=0;
                        iyPal=0;
                    }
                    index[0]=ixPal%paletteWidth+(iyPal%paletteHeight)*paletteWidth;
                    index[1]=(ixPal+1)%paletteWidth+(iyPal%paletteHeight)*paletteWidth;
                    index[2]=ixPal%paletteWidth+((iyPal+1)%paletteHeight)*paletteWidth;
                    index[3]=(ixPal+1)%paletteWidth+((iyPal+1)%paletteHeight)*paletteWidth;
                }
                else
                {
                    index[0]=(int)col0Interior_+(int)row0Interior_*paletteWidth+ixPal%(paletteWidth-(int)col0Interior_)+(iyPal%(paletteHeight-(int)row0Interior_))*paletteWidth;
                    index[1]=(int)col0Interior_+(int)row0Interior_*paletteWidth+(ixPal+1)%(paletteWidth-(int)col0Interior_)+(iyPal%(paletteHeight-(int)row0Interior_))*paletteWidth;
                    index[2]=(int)col0Interior_+(int)row0Interior_*paletteWidth+ixPal%(paletteWidth-(int)col0Interior_)+((iyPal+1)%(paletteHeight-(int)row0Interior_))*paletteWidth;
                    index[3]=(int)col0Interior_+(int)row0Interior_*paletteWidth+(ixPal+1)%(paletteWidth-(int)col0Interior_)+((iyPal+1)%(paletteHeight-(int)row0Interior_))*paletteWidth;
                }
                for(int i=0;i<4;++i)
                    col[i]=QColor(palette[index[i]]);
                scanline[ix]=colorInterp(col,xPal-ixPal,yPal-iyPal);
            }
        }
        emit imageOut(image);
    }
}
