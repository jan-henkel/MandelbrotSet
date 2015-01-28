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
    MandelbrotSet(): QObject(), parseOk_(false) {
        setDefaultPalette();
        parser_.setMathEval(&eval_);
    }
    ~MandelbrotSet() {}
public slots:
    void renderMandelbrot(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit);
    void renderJulia(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit, double cRe, double cIm);
    void setColorPalette(QImage colorPalette) {colorPalette_=colorPalette;}
    void parseFormula(QString str) {parser_.setString(str); if(!(parseOk_=parser_.parse())) emit parseErrorOut();}
signals:
    void imageOut(QImage image);
    void parseErrorOut();
private:
    void setDefaultPalette() {colorPalette_=QImage(256,1,QImage::Format_RGB32);
                              for(int i=0;i<256;++i)
                                  ((unsigned long*)colorPalette_.scanLine(0))[i]=(unsigned long)qRgb(i,i,0);}
    MathParser<Complex> parser_;
    MathEval<Complex> eval_;
    QImage colorPalette_;
    bool parseOk_;
};

#endif // MANDELBROTSET_H
