#include "mandelbrotset.h"

void MandelbrotSet::renderMandelbrot(double xCenter, double yCenter, int width, int height, double scale, int nIterations, double limit)
{
    int halfWidth=width/2;
    int halfHeight=height/2;
    QImage image(width,height,QImage::Format_RGB32);

    int palettewidth=colorPalette_->width();
    unsigned long *palette=reinterpret_cast<unsigned long*>(colorPalette_->scanLine(0));
    Complex *ec=mathEval_->getVarPtr('c'),*ex=mathEval_->getVarPtr('x');
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
                mathEval_->run();
                *ex=mathEval_->result();
                ++it;
            }
            if(it==nIterations)
                scanline[ix]=palette[0];
            else
                scanline[ix]=palette[1+it%(palettewidth-1)];
        }
    }
    emit imageOut(image);
}

void MandelbrotSet::renderJulia(double xCenter, double yCenter, int width, int height, double scale, int nIterations, double limit, Complex cJulia)
{
    int halfWidth=width/2;
    int halfHeight=height/2;
    QImage image(width,height,QImage::Format_RGB32);

    int palettewidth=colorPalette_->width();
    unsigned long *palette=reinterpret_cast<unsigned long*>(colorPalette_->scanLine(0));
    Complex *ec=mathEval_->getVarPtr('c'),*ex=mathEval_->getVarPtr('x');
    *ec=cJulia;
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
                mathEval_->run();
                *ex=mathEval_->result();
                ++it;
            }
            if(it==nIterations)
                scanline[ix]=palette[0];
            else
                scanline[ix]=palette[1+it%(palettewidth-1)];
        }
    }
    emit imageOut(image);
}
