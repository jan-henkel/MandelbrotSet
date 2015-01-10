#ifndef MANDELBROTMAINWINDOW_H
#define MANDELBROTMAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsPixmapItem>
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
public slots:
    void updateMandelbrotImage(QImage image);
private slots:
    void on_generatePushButton_clicked();

private:
    Ui::MandelbrotMainWindow *ui;
    QGraphicsPixmapItem mandelbrotPixmapItem;
    QPixmap pixmap;
    MathParser<Complex> parser;
    MathEval<Complex> eval;
    MandelbrotSet mandelbrotSet;
    bool added;
    QGraphicsScene scene;
};

#endif // MANDELBROTMAINWINDOW_H
