#ifndef MANDELBROTSET_H
#define MANDELBROTSET_H

#include <QImage>
#include <QObject>
#include <QColor>
#include <QString>
#include "complex.h"
#include "mathparser.h"

struct MandelbrotConfig
{
    QString formula;
    double limit;
    double centerX;
    double centerY;
    double scale;
    int nIterations;
    QString colorPaletteFileName;
    QString paletteFormulaX;
    bool col0interior;
    QString paletteFormulaY;
    bool row0interior;
    bool julia;
    double juliaRe;
    double juliaIm;
};

class MandelbrotSet : public QObject
{
    Q_OBJECT

public:
    enum ErrorCodes {FORMULA_PARSE_ERROR=1,PALETTE_XFORMULA_PARSE_ERROR=2,PALETTE_YFORMULA_PARSE_ERROR=4};
    MandelbrotSet(): QObject(), errorCode_(0) {
        setDefaultPalette();
        parser_.setMathEval(&eval_);
        paletteXparser_.setMathEval(&paletteXeval_);
        paletteYparser_.setMathEval(&paletteYeval_);
    }
    ~MandelbrotSet() {}
public slots:
    void renderMandelbrot(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit, int nPasses);
    void renderJulia(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit, int nPasses, double cRe, double cIm);
    void setColorPalette(QImage colorPalette) {colorPalette_=colorPalette;}
    void parseFormula(QString str) {parser_.setString(str); if(!parser_.parse()) {errorCode_|=FORMULA_PARSE_ERROR;} else {errorCode_&=~FORMULA_PARSE_ERROR;}}
    void parsePaletteXFormula(QString str) {paletteXparser_.setString(str); if(!paletteXparser_.parse()) {errorCode_|=PALETTE_XFORMULA_PARSE_ERROR;} else {errorCode_&=~PALETTE_XFORMULA_PARSE_ERROR;}}
    void parsePaletteYFormula(QString str) {paletteYparser_.setString(str); if(!paletteYparser_.parse()) {errorCode_|=PALETTE_YFORMULA_PARSE_ERROR;} else {errorCode_&=~PALETTE_YFORMULA_PARSE_ERROR;}}
    void setCol0Interior(bool b) {col0Interior_=b;}
    void setRow0Interior(bool b) {row0Interior_=b;}
signals:
    void imageOut(QImage image);
    void errorCodeOut(int errorCode);
private:
    void setDefaultPalette() {colorPalette_=QImage(256,1,QImage::Format_RGB32);
                              for(int i=0;i<256;++i)
                                  ((unsigned long*)colorPalette_.scanLine(0))[i]=(unsigned long)qRgb(i,i,0);}
    MathParser<Complex> parser_;
    MathParser<double> paletteXparser_;
    MathParser<double> paletteYparser_;
    MathEval<Complex> eval_;
    MathEval<double> paletteXeval_;
    MathEval<double> paletteYeval_;
    int errorCode_;
    QImage colorPalette_;
    bool col0Interior_;
    bool row0Interior_;
};

#endif // MANDELBROTSET_H
