#include "mandelbrotset.h"
#include <QMessageBox>
#include <QColor>

QRgb colorInterp(QColor col1,QColor col2,double r)
{
    return qRgb((int)((double)col1.red()*(1.-r)+(double)col2.red()*r),(int)((double)col1.green()*(1.-r)+(double)col2.green()*r),(int)((double)col1.blue()*(1.-r)+(double)col2.blue()*r));
}

void MandelbrotSet::renderMandelbrot(double xCenter, double yCenter, int width, int height, double scale, int nIterations, double limit)
{
    if(!parseOk_)
        return;
    int halfWidth=width/2;
    int halfHeight=height/2;
    QImage image(width,height,QImage::Format_RGB32);
    int palettewidth=colorPalette_.width();
    //int paletteheight=colorPalette_.height();
    unsigned long *palette=reinterpret_cast<unsigned long*>(colorPalette_.scanLine(0));
    Complex *ec=eval_.getVarPtr('c'),*ex=eval_.getVarPtr('x');
    for(int iy=0;iy<height;++iy)
    {
        unsigned long *scanline=reinterpret_cast<unsigned long*>(image.scanLine(iy));
        for(int ix=0;ix<width;++ix)
        {
            double x=(ix-halfWidth)*scale+xCenter;
            double y=(iy-halfHeight)*scale+yCenter;

            *ec=Complex(x,y);
            *ex=Complex(0,0);
            int it=0;
            while(it<nIterations && ex->norm2()<=limit)
            {
                eval_.run();
                *ex=eval_.result();
                ++it;
            }
            if(it==nIterations)
                scanline[ix]=palette[0];
            else
            {
                double dItF=1-log(log(ex->norm2())/2/log(2))/log(2);
                dItF=dItF<0?0:dItF;
                double dIt=it+dItF;
                int iIt=(int)dIt;
                double r=dIt-iIt;
                QColor col1=QColor(palette[1+(iIt*(palettewidth-1)/nIterations)%(palettewidth-1)]);
                QColor col2=QColor(palette[1+((iIt+1)*(palettewidth-1)/nIterations)%(palettewidth-1)]);
                scanline[ix]=colorInterp(col1,col2,r);
            }
        }
    }

    emit imageOut(image);
}

void MandelbrotSet::renderJulia(double xCenter, double yCenter, int width, int height, double scale, int nIterations, double limit, double cRe, double cIm)
{
    if(!parseOk_)
        return;
    int halfWidth=width/2;
    int halfHeight=height/2;
    QImage image(width,height,QImage::Format_RGB32);

    int palettewidth=colorPalette_.width();
    int paletteheight=colorPalette_.height();
    unsigned long *palette=reinterpret_cast<unsigned long*>(colorPalette_.scanLine(0));
    Complex *ec=eval_.getVarPtr('c'),*ex=eval_.getVarPtr('x');
    *ec=Complex(cRe,cIm);
    for(int iy=0;iy<height;++iy)
    {
        unsigned long *scanline=reinterpret_cast<unsigned long*>(image.scanLine(iy));
        for(int ix=0;ix<width;++ix)
        {
            double x=(ix-halfWidth)*scale+xCenter;
            double y=(iy-halfHeight)*scale+yCenter;

            *ex=Complex(x,y);
            int it=0;
            while(it<nIterations && ex->norm2()<=limit)
            {
                eval_.run();
                *ex=eval_.result();
                ++it;
            }
            if(it==nIterations)
                scanline[ix]=palette[0];
            else
                scanline[ix]=palette[1+it%(palettewidth-1)+(((int)((ex->norm2()-limit)/limit)*paletteheight)%paletteheight)*palettewidth];
        }
    }
    emit imageOut(image);
}
