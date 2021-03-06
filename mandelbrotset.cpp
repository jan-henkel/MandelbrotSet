#include "mandelbrotset.h"
#include <QMessageBox>
#include <QColor>
const qint32 REPORT_LINES_RENDERED=32;

inline QRgb colorInterp(QColor col[4],double x,double y)
{
    return qRgb(
            (int)(((double)col[0].red()*(1.-x)+(double)col[1].red()*x)*(1.-y)+((double)col[2].red()*(1.-x)+(double)col[3].red()*x)*y),
            (int)(((double)col[0].green()*(1.-x)+(double)col[1].green()*x)*(1.-y)+((double)col[2].green()*(1.-x)+(double)col[3].green()*x)*y),
            (int)(((double)col[0].blue()*(1.-x)+(double)col[1].blue()*x)*(1.-y)+((double)col[2].blue()*(1.-x)+(double)col[3].blue()*x)*y)
            );
}

void MandelbrotSet::renderMandelbrot(double xCenter, double yCenter, qint32 width, qint32 height, double scale, qint32 nIterations, double limit, qint32 nPasses)
{
    if(--cancel_>0)
        return;
    else
        cancel_=0;
    emit errorCodeOut(errorCode_);
    if(errorCode_)
        return;
    qint32 halfWidth=width/2;
    qint32 halfHeight=height/2;
    QImage image(width,height,QImage::Format_RGB32);
    qint32 paletteWidth=colorPalette_.width();
    qint32 paletteHeight=colorPalette_.height();
    quint32 *palette=reinterpret_cast<quint32*>(colorPalette_.scanLine(0));
    //z=z(n), c=std::complex<double>(x,y) with coordinates x and y on the complex plane corresponding to points of the image
    std::complex<double> *ec=eval_.getVarPtr('c'),*ez=eval_.getVarPtr('z');
    //imaginary unit i
    (*eval_.getVarPtr('i'))=std::complex<double>(0.0,1.0);
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
    const qint32 upperLimit=(1<<(sizeof(int)*8-2));
    for(qint32 pass=0;pass<nPasses;++pass)
    {
        qint32 nIt=nIterations>>(2*(nPasses-pass-1));
        for(qint32 iy=0;iy<height;++iy)
        {
            quint32 *scanline=reinterpret_cast<quint32*>(image.scanLine(iy));
            if(cancel_)
            {
                --cancel_;
                return;
            }
            for(qint32 ix=0;ix<width;++ix)
            {
                double x=(ix-halfWidth)*scale+xCenter;
                double y=(iy-halfHeight)*scale+yCenter;

                *ec=std::complex<double>(x,y);
                *ez=std::complex<double>(0,0);
                qint32 it=0;
                while(it<nIt && (ez->real()*ez->real()+ez->imag()*ez->imag())<=limit)
                {
                    eval_.run();
                    *ez=eval_.result();
                    ++it;
                }
                *e1s=*e2s=ez->real();
                *e1t=*e2t=ez->imag();
                *e1u=*e2u=ec->real();
                *e1v=*e2v=ec->imag();
                *e1n=*e2n=(double)it;
                double xPal,yPal;
                qint32 ixPal,iyPal;
                paletteXeval_.run();
                paletteYeval_.run();
                xPal=paletteXeval_.result();
                yPal=paletteYeval_.result();
                xPal=(xPal<0 || xPal>upperLimit || xPal!=xPal)?0:xPal;
                yPal=(yPal<0 || yPal>upperLimit || yPal!=yPal)?0:yPal;
                ixPal=(int)xPal;
                iyPal=(int)yPal;
                qint32 index[4];
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
                for(qint32 i=0;i<4;++i)
                    col[i]=QColor(palette[index[i]]);
                scanline[ix]=colorInterp(col,xPal-ixPal,yPal-iyPal);
            }
            if(iy%REPORT_LINES_RENDERED==0)
                emit linesRendered(height*pass+iy+1);
        }
        emit linesRendered(height*(pass+1));
        emit imageOut(image);
    }
}

void MandelbrotSet::renderJulia(double xCenter, double yCenter, qint32 width, qint32 height, double scale, qint32 nIterations, double limit, qint32 nPasses, double cRe, double cIm)
{
    if(--cancel_>0)
        return;
    else
        cancel_=0;
    emit errorCodeOut(errorCode_);
    if(errorCode_)
        return;
    qint32 halfWidth=width/2;
    qint32 halfHeight=height/2;
    QImage image(width,height,QImage::Format_RGB32);
    qint32 paletteWidth=colorPalette_.width();
    qint32 paletteHeight=colorPalette_.height();
    quint32 *palette=reinterpret_cast<quint32*>(colorPalette_.scanLine(0));
    std::complex<double> *ec=eval_.getVarPtr('c'),*ez=eval_.getVarPtr('z');
    //imaginary unit i
    (*eval_.getVarPtr('i'))=std::complex<double>(0.0,1.0);
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

    const qint32 upperLimit=(1<<(sizeof(int)*8-2));
    *e1m=*e2m=(double)nIterations;
    *e1l=*e2l=limit;
    *e1w=*e2w=(double)paletteWidth;
    *e1h=*e2h=(double)paletteHeight;
    *ec=std::complex<double>(cRe,cIm);
    col0Interior_&=(paletteWidth>1);
    row0Interior_&=(paletteHeight>1);
    for(qint32 pass=0;pass<nPasses;++pass)
    {
        qint32 nIt=nIterations>>(2*(nPasses-pass-1));
        for(qint32 iy=0;iy<height;++iy)
        {
            quint32 *scanline=reinterpret_cast<quint32*>(image.scanLine(iy));
            if(cancel_)
            {
                --cancel_;
                return;
            }
            for(qint32 ix=0;ix<width;++ix)
            {
                double x=(ix-halfWidth)*scale+xCenter;
                double y=(iy-halfHeight)*scale+yCenter;

                *ez=std::complex<double>(x,y);
                qint32 it=0;
                while(it<nIt && (ez->real()*ez->real()+ez->imag()*ez->imag())<=limit)
                {
                    eval_.run();
                    *ez=eval_.result();
                    ++it;
                }
                *e1s=*e2s=ez->real();
                *e1t=*e2t=ez->imag();
                *e1u=*e2u=x;
                *e1v=*e2v=y;
                *e1n=*e2n=(double)it;
                double xPal,yPal;
                qint32 ixPal,iyPal;
                paletteXeval_.run();
                paletteYeval_.run();
                xPal=paletteXeval_.result();
                yPal=paletteYeval_.result();
                xPal=(xPal<0 || xPal>upperLimit || xPal!=xPal)?0:xPal;
                yPal=(yPal<0 || yPal>upperLimit || yPal!=yPal)?0:yPal;
                ixPal=(int)xPal;
                iyPal=(int)yPal;
                qint32 index[4];
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
                for(qint32 i=0;i<4;++i)
                    col[i]=QColor(palette[index[i]]);
                scanline[ix]=colorInterp(col,xPal-ixPal,yPal-iyPal);
            }
            if(iy%REPORT_LINES_RENDERED==0)
                emit linesRendered(height*pass+iy+1);
        }
        emit linesRendered(height*(pass+1));
        emit imageOut(image);
    }
}
