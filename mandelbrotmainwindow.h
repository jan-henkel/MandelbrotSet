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
#include <complex>
#include <map>
#include <utility>
#include <QTimer>

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
    //signals for rendering images in another thread
    void renderMandelbrot(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit, int nPasses);
    void renderJulia(double xCenter,double yCenter, int width, int height, double scale, int nIterations, double limit, int nPasses, double cRe, double cIm);
    //signals for changing settings of the MandelbrotSet instance which takes care of calculation and rendering
    void parseFormula(QString formula);
    void parsePaletteXFormula(QString formula);
    void parsePaletteYFormula(QString formula);
    void setColorPalette(QImage palette);
    void setCol0Interior(bool b);
    void setRow0Interior(bool b);
public slots:
    //processing of incoming signals from worker thread
    void updateImage(QImage image);
    void receiveErrorCode(int errorCode);
protected:
    virtual void resizeEvent(QResizeEvent *e);
    //event filter to intercept mouse events on the render area
    bool eventFilter(QObject *target, QEvent *e);
private slots:
    //UI interaction slots
    //note: UI code is still messy
    //to do: thorough model-view-controller implementation
    void on_nameComboBox_activated(const QString &str);
    void on_saveConfigPushButton_clicked();
    void on_restoreConfigPushButton_clicked();
    void on_deleteConfigPushButton_clicked();
    void on_setColorPalettePushButton_clicked();
    void on_mandelbrotRadioButton_toggled(bool checked);
    void on_applyPushButton_clicked();
    void on_saveImagePushButton_clicked();
    void on_formulaLineEdit_textEdited(const QString &);
    void on_limitLineEdit_textEdited(const QString &);
    void on_xLineEdit_textEdited(const QString &);
    void on_yLineEdit_textEdited(const QString &);
    void on_scaleLineEdit_textEdited(const QString &);
    void on_iterationsLineEdit_textEdited(const QString &);
    void on_paletteFormulaXLineEdit_textEdited(const QString &);
    void on_col0CheckBox_clicked();
    void on_paletteFormulaYLineEdit_textEdited(const QString &);
    void on_row0CheckBox_clicked();
    void on_mandelbrotRadioButton_clicked();
    void on_juliaRadioButton_clicked();
    void on_juliaXLineEdit_textEdited(const QString &);
    void on_juliaYLineEdit_textEdited(const QString &);
    //progress bar slot
    void on_renderProgressBar_valueChanged(int value);

    //render image slot, sometimes called as normal member
    void renderImage();


private:
    Ui::MandelbrotMainWindow *ui;

    //core calculation and rendering engine, works on seperate thread
    MandelbrotSet mandelbrotSet;
    QThread workerThread;
    static const int PASSES;

    //contents of render area
    QPixmap mandelbrotPixmap;
    QGraphicsScene mandelbrotScene;
    QGraphicsPixmapItem mandelbrotPixmapItem;

    //render area drag and zoom controls
    QPoint dragClickPos;
    QPoint preDragOffset;
    QPoint zoomClickPos;
    QGraphicsRectItem zoomRect;
    static const int MIN_ZOOM_WIDTH;
    static const int MIN_ZOOM_HEIGHT;
    static const int MIN_DRAG_DISTANCE_SQUARED;
    void zoomToRect(QRectF rect);
    void moveByOffset(QPoint offset);

    //set of configurations addressable by their name
    std::map<QString,MandelbrotConfig> configurations;
    //current config
    QString currentConfigName;
    MandelbrotConfig currentConfig;
    QImage currentColorPalette;

    //config management
    void addConfig(QString name,const MandelbrotConfig& config);
    void applyCurrentConfig();
    void saveCurrentConfig();
    void restoreCurrentConfig();
    void deleteCurrentConfig();
    void readConfigs();
    void writeConfigs();

    //update UI contents (barring the render area) to reflect config
    void updateConfigUI();
    void updateColorPalettePreview();

    //update config to reflect UI contents, return an error code specifying which assignments went wrong
    int setConfigToUIContents();

    //default config
    static const QString DEFAULT_CONFIG_NAME;
    static const MandelbrotConfig DEFAULT_CONFIG;
    //modified coloring formula to reduce banding
    static const QString DEFAULT_CONFIG_SMOOTH_COLORING_NAME;
    static const MandelbrotConfig DEFAULT_CONFIG_SMOOTH_COLORING;

    static const int DEFAULT_ITERATIONS;
    static const double DEFAULT_SCALE;
    static const double DEFAULT_LIMIT;

    //default color palette
    QImage defaultPalette;
    void generateDefaultPalette();

    //while resizing or zooming with the mouse wheel, a single shot timer is continually reset.
    //upon running out, the image is rerendered. this is to prevent large amounts of rerender calls from piling up.
    QTimer delayedRenderTimer;
};

#endif // MANDELBROTMAINWINDOW_H
