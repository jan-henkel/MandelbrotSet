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
#include <map>
#include <utility>
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
    void renderMandelbrot(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit, int nPasses);
    void renderJulia(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit, int nPasses, double cRe, double cIm);
    void parseFormula(QString formula);
    void parsePaletteXFormula(QString formula);
    void parsePaletteYFormula(QString formula);
    void setColorPalette(QImage palette);
    void setCol0Interior(bool b);
    void setRow0Interior(bool b);
public slots:
    void updateImage(QImage image);
    void receiveErrorCode(int errorCode);
protected:
    virtual void resizeEvent(QResizeEvent *e);
private slots:
    void updateImageOffsetDrag(QPoint newOffset);
    void updateImageOffsetRelease(QPoint newOffset);
    void updateImageViewRect(QRectF viewRect);
    void on_setColorPalettePushButton_clicked();
    void on_nameComboBox_activated(const QString &str);
    void on_saveConfigPushButton_clicked();
    void on_saveImagePushButton_clicked();
    void on_restoreConfigPushButton_clicked();
    void on_deleteConfigPushButton_clicked();
    void on_applyPushButton_clicked();
    void on_mandelbrotRadioButton_toggled(bool checked);

private:
    void renderImage();
    void renderMandelbrot();
    void renderJulia();
    void updateConfigUI();
    int setConfigToUIContents();

    void applyConfig();
    void saveConfig();
    void restoreConfig();
    void deleteConfig();
    void selectColorPalette();
    void saveImage();

    void readConfigs();
    void writeConfigs();
    Ui::MandelbrotMainWindow *ui;
    const QString STANDARD_CONFIG_NAME;
    const MandelbrotConfig STANDARD_CONFIG;
    std::map<QString,MandelbrotConfig> configurations;
    std::pair<QString,MandelbrotConfig> currentConfig;
    QThread renderThread;
    QGraphicsPixmapItem mandelbrotPixmapItem;
    QPixmap pixmap;
    MandelbrotSet mandelbrotSet;
    QGraphicsScene scene;
    bool uiChanged;
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
