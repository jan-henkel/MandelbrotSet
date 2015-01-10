#include "mandelbrotset.h"

void MandelbrotSet::render(double xCenter, double yCenter, int width, int height, double scale, int nIterations, double limit)
{
    int halfWidth=width/2;
    int halfHeight=height/2;
    QImage image(width,height,QImage::Format_RGB32);

    int palettewidth=colorPalette_->width();
    unsigned long *palette=reinterpret_cast<unsigned long*>(colorPalette_->scanLine(0));
    for(int iy=0;iy<height;++iy)
    {
        unsigned long *scanline=reinterpret_cast<unsigned long*>(image.scanLine(iy));
        for(int ix=0;ix<width;++ix)
        {
            double x=(ix-halfWidth)*scale+xCenter;
            double y=(iy-halfHeight)*scale+yCenter;
            mathEval_->setVar('c',Complex(x,y));
            mathEval_->setVar('x',Complex(0,0));
            int it=0;
            while(it<nIterations && mathEval_->getVar('x').norm2()<=limit)
            {
                mathEval_->run();
                mathEval_->setVar('x',mathEval_->pop());
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
