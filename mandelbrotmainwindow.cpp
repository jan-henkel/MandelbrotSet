#include "mandelbrotmainwindow.h"
#include "ui_mandelbrotmainwindow.h"
const double DefaultCenterX = -0.637011f;
const double DefaultCenterY = -0.0395159f;
const double DefaultScale = 0.00403897f;

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
    scene.setSceneRect(0,0,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height());
    ui->mandelbrotGraphicsView->setScene(&scene);
    scene.addItem(&mandelbrotPixmapItem);
    QObject::connect(&mandelbrotSet,SIGNAL(imageOut(QImage)),this,SLOT(updateMandelbrotImage(QImage)),Qt::QueuedConnection);
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateOffsetDrag(QPoint)),this,SLOT(updateImageOffsetDrag(QPoint)));
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateOffsetRelease(QPoint)),this,SLOT(updateImageOffsetRelease(QPoint)));
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateViewRect(QRectF)),this,SLOT(updateImageViewRect(QRectF)));
    QObject::connect(this,SIGNAL(renderMandelbrot(double,double,int,int,double,int,double)),&mandelbrotSet,SLOT(renderMandelbrot(double,double,int,int,double,int,double)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(renderJulia(double,double,int,int,double,int,double,double,double)),&mandelbrotSet,SLOT(renderJulia(double,double,int,int,double,int,double,double,double)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(parseFormula(QString)),&mandelbrotSet,SLOT(parseFormula(QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(setColorPalette(QImage)),&mandelbrotSet,SLOT(setColorPalette(QImage)),Qt::QueuedConnection);
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
    updateCoordinatesUI();
    renderMandelbrot();
}

void MandelbrotMainWindow::updateImageViewRect(QRectF viewRect)
{
    centerX+=scale*(viewRect.x()+viewRect.width()/2-ui->mandelbrotGraphicsView->width()/2);
    centerY+=scale*(viewRect.y()+viewRect.height()/2-ui->mandelbrotGraphicsView->height()/2);
    scale*=viewRect.width()/ui->mandelbrotGraphicsView->width();
    updateCoordinatesUI();
    renderMandelbrot();
}

void MandelbrotMainWindow::updateCoordinatesUI()
{
    ui->xLineEdit->setText(QString::number(centerX));
    ui->yLineEdit->setText(QString::number(centerY));
    ui->scaleLineEdit->setText(QString::number(scale));
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
    w=(h*width()/height()<=w)?w:h*width()/height();
    h=(w*height()/width()<=h)?h:w*height()/width();
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
    ui->mandelbrotGraphicsView->update();
}

void MandelbrotMainWindow::renderMandelbrot()
{
    emit renderMandelbrot(centerX,centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),scale,nIterations/4,limit);
    emit renderMandelbrot(centerX,centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),scale,nIterations,limit);
}
void MandelbrotMainWindow::renderJulia()
{

}

void MandelbrotMainWindow::on_setColorPalettePushButton_clicked()
{
    QImage palette;
    QString strFilename=QFileDialog::getOpenFileName(0,"Select color palette");
    if(strFilename!="" && palette.load(strFilename))
        emit setColorPalette(palette);
}

void MandelbrotMainWindow::on_generateMandelbrotPushButton_clicked()
{
    emit parseFormula(ui->formulaComboBox->currentText());
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
    if(ok)
        renderMandelbrot();
}

void MandelbrotMainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    scene.setSceneRect(0,0,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height());
    mandelbrotPixmapItem.setPos(ui->mandelbrotGraphicsView->width()/2-mandelbrotPixmapItem.pixmap().width()/2,ui->mandelbrotGraphicsView->height()/2-mandelbrotPixmapItem.pixmap().height()/2);
    renderMandelbrot();
}
