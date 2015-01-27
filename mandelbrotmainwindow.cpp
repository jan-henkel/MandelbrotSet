#include "mandelbrotmainwindow.h"
#include "ui_mandelbrotmainwindow.h"
const double DefaultCenterX = -0.637011f;
const double DefaultCenterY = -0.0395159f;
const double DefaultScale = 0.00403897f;

#include <QMessageBox>

MandelbrotMainWindow::MandelbrotMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MandelbrotMainWindow),
    added(false),
    scale(DefaultScale),
    centerX(DefaultCenterX),
    centerY(DefaultCenterY),
    nIterations(100)
{
    mandelbrotSet.moveToThread(&renderThread);
    renderThread.start();
    ui->setupUi(this);
    mandelbrotPixmapItem.setPixmap(pixmap);
    parser.setMathEval(&eval);
    mandelbrotSet.setMathEval(&eval);
    scene.setSceneRect(0,0,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height());
    ui->mandelbrotGraphicsView->setScene(&scene);
    scene.addItem(&mandelbrotPixmapItem);
    QObject::connect(&mandelbrotSet,SIGNAL(imageOut(QImage)),this,SLOT(updateMandelbrotImage(QImage)),Qt::QueuedConnection);
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateOffsetDrag(QPoint)),this,SLOT(updateImageOffsetDrag(QPoint)));
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateOffsetRelease(QPoint)),this,SLOT(updateImageOffsetRelease(QPoint)));
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateViewRect(QRectF)),this,SLOT(updateImageViewRect(QRectF)));
    QObject::connect(this,SIGNAL(renderMandelbrot(double,double,int,int,double,int,double)),&mandelbrotSet,SLOT(renderMandelbrot(double,double,int,int,double,int,double)));
    if(mandelbrotSet.thread()==this->thread())
    {QMessageBox msgBox;
        msgBox.setText("Same thread.");
        msgBox.exec();}
}

MandelbrotMainWindow::~MandelbrotMainWindow()
{
    scene.removeItem(&mandelbrotPixmapItem);
    delete ui;
}

void MandelbrotMainWindow::updateImageOffsetDrag(QPoint newOffset)
{
    mandelbrotPixmapItem.setOffset(newOffset);
    ui->mandelbrotGraphicsView->update();
}

void MandelbrotMainWindow::updateImageOffsetRelease(QPoint newOffset)
{
    centerX-=scale*newOffset.rx();
    centerY-=scale*newOffset.ry();
    renderMandelbrot();
}

void MandelbrotMainWindow::updateImageViewRect(QRectF viewRect)
{
    centerX+=scale*(viewRect.x()+viewRect.width()/2-ui->mandelbrotGraphicsView->width()/2);
    centerY+=scale*(viewRect.y()+viewRect.height()/2-ui->mandelbrotGraphicsView->height()/2);
    scale*=viewRect.width()/ui->mandelbrotGraphicsView->width();
    renderMandelbrot();
}

void ScrollableGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
        dragClickPos=event->pos();
    if(event->button()==Qt::RightButton)
    {
        zoomClickPos=event->pos();
        int w,h;
        w=minZoomWidth;
        h=minZoomHeight;
        if(this->width()>this->height())
            w=h*width()/height();
        else
            h=w*height()/width();
        zoomRect.setRect(event->pos().x(),event->pos().y(),w,h);
        this->scene()->addItem(&zoomRect);
        this->update();
    }
}

void ScrollableGraphicsView::updateZoomRect(QPoint p1, QPoint p2)
{
    QPoint d=p2-p1;
    int w,h;
    w=(d.x()<minZoomWidth)?minZoomWidth:d.x();
    h=(d.y()<minZoomHeight)?minZoomHeight:d.y();
    if(this->width()>this->height())
        w=h*width()/height();
    else
        h=w*height()/width();
    zoomRect.setRect(zoomClickPos.x(),zoomClickPos.y(),w,h);
    this->update();
}

void ScrollableGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
        emit updateOffsetDrag(event->pos()-dragClickPos);
    if(event->buttons() & Qt::RightButton)
        emit updateZoomRect(zoomClickPos,event->pos());
}

void ScrollableGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        emit updateOffsetRelease(event->pos()-dragClickPos);
    if(event->button() == Qt::RightButton)
    {
        this->scene()->removeItem(&zoomRect);
        emit updateViewRect(this->zoomRect.rect());
    }
}

void MandelbrotMainWindow::updateMandelbrotImage(QImage image)
{
    pixmap=QPixmap::fromImage(image);
    mandelbrotPixmapItem.setPos(0,0);
    mandelbrotPixmapItem.setOffset(0,0);
    mandelbrotPixmapItem.setPixmap(pixmap);
    /*if(!added)
    {
        ui->mandelbrotGraphicsView->scene()->addItem(&mandelbrotPixmapItem);
        added=true;
    }*/
    ui->mandelbrotGraphicsView->update();
}

void MandelbrotMainWindow::renderMandelbrot()
{
    /*QMetaObject::invokeMethod(&mandelbrotSet,"renderMandelbrot",Qt::QueuedConnection,Q_ARG(double,centerX),Q_ARG(double,centerY),Q_ARG(int,ui->mandelbrotGraphicsView->width()),Q_ARG(int,ui->mandelbrotGraphicsView->height()),Q_ARG(double,scale),Q_ARG(int,nIterations/8),Q_ARG(double,limit));
    QMetaObject::invokeMethod(&mandelbrotSet,"renderMandelbrot",Qt::QueuedConnection,Q_ARG(double,centerX),Q_ARG(double,centerY),Q_ARG(int,ui->mandelbrotGraphicsView->width()),Q_ARG(int,ui->mandelbrotGraphicsView->height()),Q_ARG(double,scale),Q_ARG(int,nIterations/4),Q_ARG(double,limit));
    QMetaObject::invokeMethod(&mandelbrotSet,"renderMandelbrot",Qt::QueuedConnection,Q_ARG(double,centerX),Q_ARG(double,centerY),Q_ARG(int,ui->mandelbrotGraphicsView->width()),Q_ARG(int,ui->mandelbrotGraphicsView->height()),Q_ARG(double,scale),Q_ARG(int,nIterations/2),Q_ARG(double,limit));
    QMetaObject::invokeMethod(&mandelbrotSet,"renderMandelbrot",Qt::QueuedConnection,Q_ARG(double,centerX),Q_ARG(double,centerY),Q_ARG(int,ui->mandelbrotGraphicsView->width()),Q_ARG(int,ui->mandelbrotGraphicsView->height()),Q_ARG(double,scale),Q_ARG(int,nIterations),Q_ARG(double,limit));
    */
    emit renderMandelbrot(centerX,centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),scale,nIterations/8,limit);
    emit renderMandelbrot(centerX,centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),scale,nIterations/4,limit);
    emit renderMandelbrot(centerX,centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),scale,nIterations,limit);
    /*mandelbrotSet.renderMandelbrot(centerX,centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),scale,nIterations/8,limit);
    mandelbrotSet.renderMandelbrot(centerX,centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),scale,nIterations/4,limit);
    mandelbrotSet.renderMandelbrot(centerX,centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),scale,nIterations/2,limit);
    mandelbrotSet.renderMandelbrot(centerX,centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),scale,nIterations,limit);*/
}
void MandelbrotMainWindow::renderJulia()
{

}

void MandelbrotMainWindow::on_setColorPalettePushButton_clicked()
{
    palette.load(QFileDialog::getOpenFileName());
    mandelbrotSet.setColorPalette(&palette);
}

void MandelbrotMainWindow::on_generateMandelbrotPushButton_clicked()
{
    parser.setString(ui->formulaComboBox->currentText());
    bool ok;
    limit=ui->limitLineEdit->text().toDouble(&ok);
    if(ok)
        centerX=ui->xLineEdit->text().toDouble(&ok);
    if(ok)
        centerY=ui->yLineEdit->text().toDouble(&ok);
    if(ok)
        scale=ui->scaleLineEdit->text().toDouble(&ok);
    if(ok)
        nIterations=ui->iterationsLineEdit->text().toDouble(&ok);
    if(parser.parse() && ok)
    {
        renderMandelbrot();
    }
}

void MandelbrotMainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    scene.setSceneRect(0,0,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height());
    mandelbrotPixmapItem.setPos(ui->mandelbrotGraphicsView->width()/2-mandelbrotPixmapItem.pixmap().width()/2,ui->mandelbrotGraphicsView->height()/2-mandelbrotPixmapItem.pixmap().height()/2);

}
