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

//MandelbrotSet class renders an area of the set of complex numbers z whose norm squared stays below a given limit
//when iterating the assignment z=f(z) where f is a user defined formula.
//Arbitrary images can be used as color palettes. The coloring is determined by further user defined formulas
//yielding x and y coordinates on the color palette depending on the following parameters:
//n: number of iterations before reaching the limit, m: maximum number of iterations
//s,t: real and imaginary components of z, u,v: real and imaginary components corresponding to the current pixel,
//h,w: height and width of the color palette in pixels

class MandelbrotSet : public QObject
{
    Q_OBJECT

public:
    enum ErrorCodes {FORMULA_PARSE_ERROR=1,PALETTE_XFORMULA_PARSE_ERROR=2,PALETTE_YFORMULA_PARSE_ERROR=4};
    MandelbrotSet(): QObject(), errorCode_(0), cancel_(false) {
        parser_.setMathEval(&eval_);
        paletteXparser_.setMathEval(&paletteXeval_);
        paletteYparser_.setMathEval(&paletteYeval_);
    }
    ~MandelbrotSet() {}
    void cancel() {++cancel_;}
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
    void linesRendered(int lines);
private:
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
    int cancel_;
};

#endif // MANDELBROTSET_H
