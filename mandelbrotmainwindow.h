#ifndef MANDELBROTMAINWINDOW_H
#define MANDELBROTMAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QThread>
#include <QMouseEvent>
#include <QFileDialog>
#include "mandelbrotset.h"
#include "mathparser.h"
#include "complex.h"
namespace Ui {
class MandelbrotMainWindow;
}

class MandelbrotMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MandelbrotMainWindow(QWidget *parent = 0);
    ~MandelbrotMainWindow();
signals:
    void renderMandelbrot(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit);
    void renderJulia(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit, double cRe, double cIm);
    void parseFormula(QString strFormula);
    void setColorPalette(QImage palette);
public slots:
    void updateMandelbrotImage(QImage image);
protected:
    virtual void resizeEvent(QResizeEvent *e);
private slots:
    void updateImageOffsetDrag(QPoint newOffset);
    void updateImageOffsetRelease(QPoint newOffset);
    void updateImageViewRect(QRectF viewRect);
    void on_setColorPalettePushButton_clicked();
    void on_generateMandelbrotPushButton_clicked();

private:
    void renderMandelbrot();
    void renderJulia();
    void updateCoordinatesUI();
    Ui::MandelbrotMainWindow *ui;
    QThread renderThread;
    QGraphicsPixmapItem mandelbrotPixmapItem;
    QPixmap pixmap;
    MandelbrotSet mandelbrotSet;
    bool added;
    QGraphicsScene scene;
    double centerX,centerY;
    double scale;
    double limit;
    int nIterations;
};

class ScrollableGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    ScrollableGraphicsView(QWidget *parent=0):QGraphicsView(parent),minZoomWidth(10),minZoomHeight(10) {}
signals:
    void updateOffsetDrag(QPoint dOffset);
    void updateOffsetRelease(QPoint dOffset);
    void updateViewRect(QRectF);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void updateZoomRect(QPoint p1,QPoint p2);
private:
    QPoint dragClickPos;
    QPoint zoomClickPos;
    QGraphicsRectItem zoomRect;
    int minZoomWidth;
    int minZoomHeight;
};

#endif // MANDELBROTMAINWINDOW_H
