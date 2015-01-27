#ifndef MANDELBROTSET_H
#define MANDELBROTSET_H

#include <QImage>
#include <QObject>
#include <QColor>
#include "complex.h"
#include "mathparser.h"

class MandelbrotSet : public QObject
{
    Q_OBJECT

public:
    MandelbrotSet(): QObject(), mathEval_(0), defaultPalette(true), inUse(false) {
        colorPalette_=new QImage(256,1,QImage::Format_RGB32);
        for(int i=0;i<256;++i)
            ((unsigned long*)colorPalette_->scanLine(0))[i]=(unsigned long)qRgb(i,i,0);
    }
    MandelbrotSet(MathEval<Complex>* mathEval, QImage* colorPalette): mathEval_(mathEval), colorPalette_(colorPalette), defaultPalette(false) {}
    ~MandelbrotSet() {}
    void setMathEval(MathEval<Complex>* mathEval) {mathEval_=mathEval;}
    void setColorPalette(QImage* colorPalette) {colorPalette_=colorPalette; defaultPalette=false;}
public slots:
    void renderMandelbrot(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit);
    void renderJulia(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit, Complex cJulia);

signals:
    void imageOut(QImage image);

private:
    MathEval<Complex>* mathEval_;
    QImage* colorPalette_;
    bool defaultPalette;
    bool inUse;
};

#endif // MANDELBROTSET_H
