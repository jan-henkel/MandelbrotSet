#include "mandelbrotmainwindow.h"
#include "ui_mandelbrotmainwindow.h"
const double DefaultCenterX = -0.637011f;
const double DefaultCenterY = -0.0395159f;
const double DefaultScale = 0.00403897f;

MandelbrotMainWindow::MandelbrotMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MandelbrotMainWindow),
    added(false)
{
    ui->setupUi(this);
    mandelbrotPixmapItem.setPixmap(pixmap);
    parser.setMathEval(&eval);
    mandelbrotSet.setMathEval(&eval);
    ui->mandelbrotGraphicsView->setScene(&scene);
    scene.addItem(&mandelbrotPixmapItem);
    QObject::connect(&mandelbrotSet,SIGNAL(imageOut(QImage)),this,SLOT(updateMandelbrotImage(QImage)));
}

MandelbrotMainWindow::~MandelbrotMainWindow()
{
    scene.removeItem(&mandelbrotPixmapItem);
    delete ui;
}

void MandelbrotMainWindow::updateMandelbrotImage(QImage image)
{
    pixmap=QPixmap::fromImage(image);
    mandelbrotPixmapItem.setPixmap(pixmap);
    /*if(!added)
    {
        ui->mandelbrotGraphicsView->scene()->addItem(&mandelbrotPixmapItem);
        added=true;
    }*/
    ui->mandelbrotGraphicsView->update();
}

void MandelbrotMainWindow::on_generatePushButton_clicked()
{
    parser.setString(ui->formulaLineEdit->text());
    bool ok;
    double limit=ui->limitLineEdit->text().toDouble(&ok);
    if(parser.parse() && ok)
    {
        mandelbrotSet.render(DefaultCenterX,DefaultCenterY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),DefaultScale,20,limit);
    }
}
