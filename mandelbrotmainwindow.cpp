#include "mandelbrotmainwindow.h"
#include "ui_mandelbrotmainwindow.h"
#include <QFile>
#include <QTextStream>

const QString MandelbrotMainWindow::STANDARD_CONFIG_NAME="Standard Mandelbrot";
const MandelbrotConfig MandelbrotMainWindow::STANDARD_CONFIG=
        {"z^2+c",       //formula
        4.0,            //limit
        -0.637011f,     //centerX
        -0.0395159f,    //centerY
        0.00403897f,    //scale
        100,            //nIterations
        "",             //colorPaletteFileName
        "n/m*(w-1)",    //paletteFormulaX
        true,           //col0interior
        "0",            //paletteFormulaY
        false,          //row0interior
        false,          //julia
        0.0,            //juliaRe
        0.0             //juliaIm
        };
const QString MandelbrotMainWindow::STANDARD_CONFIG_SMOOTH_COLORING_NAME="Standard Mandelbrot (smooth coloring)";
const MandelbrotConfig MandelbrotMainWindow::STANDARD_CONFIG_SMOOTH_COLORING=
        {"z^2+c",                                           //formula
        4.0,                                                //limit
        -0.637011f,                                         //centerX
        -0.0395159f,                                        //centerY
        0.00403897f,                                        //scale
        100,                                                //nIterations
        "",                                                 //colorPaletteFileName
        "(n+1-log(log(s^2+t^2)/log(4))/log(2))/m*(w-1)",    //paletteFormulaX
        true,                                               //col0interior
        "0",                                                //paletteFormulaY
        false,                                              //row0interior
        false,                                              //julia
        0.0,                                                //juliaRe
        0.0                                                 //juliaIm
        };


MandelbrotMainWindow::MandelbrotMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MandelbrotMainWindow)
{
    mandelbrotSet.moveToThread(&renderThread);
    renderThread.start();
    ui->setupUi(this);
    resizeTimer.setSingleShot(true);
    generateDefaultPalette();
    mandelbrotPixmapItem.setPixmap(pixmap);
    scene.setSceneRect(0,0,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height());
    ui->mandelbrotGraphicsView->setScene(&scene);
    scene.addItem(&mandelbrotPixmapItem);
    QObject::connect(&mandelbrotSet,SIGNAL(imageOut(QImage)),this,SLOT(updateImage(QImage)),Qt::QueuedConnection);
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateOffsetDrag(QPoint)),this,SLOT(updateImageOffsetDrag(QPoint)));
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateOffsetRelease(QPoint)),this,SLOT(updateImageOffsetRelease(QPoint)));
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateViewRect(QRectF)),this,SLOT(updateImageViewRect(QRectF)));
    QObject::connect(&resizeTimer,SIGNAL(timeout()),this,SLOT(resizeTimerExpired()));
    QObject::connect(this,SIGNAL(renderMandelbrot(double,double,int,int,double,int,double,int)),&mandelbrotSet,SLOT(renderMandelbrot(double,double,int,int,double,int,double,int)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(renderJulia(double,double,int,int,double,int,double,int,double,double)),&mandelbrotSet,SLOT(renderJulia(double,double,int,int,double,int,double,int,double,double)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(parseFormula(QString)),&mandelbrotSet,SLOT(parseFormula(QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(parsePaletteXFormula(QString)),&mandelbrotSet,SLOT(parsePaletteXFormula(QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(parsePaletteYFormula(QString)),&mandelbrotSet,SLOT(parsePaletteYFormula(QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(setCol0Interior(bool)),&mandelbrotSet,SLOT(setCol0Interior(bool)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(setRow0Interior(bool)),&mandelbrotSet,SLOT(setRow0Interior(bool)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(setColorPalette(QImage)),&mandelbrotSet,SLOT(setColorPalette(QImage)),Qt::QueuedConnection);
    readConfigs();
    addConfig(STANDARD_CONFIG_NAME,STANDARD_CONFIG);
    addConfig(STANDARD_CONFIG_SMOOTH_COLORING_NAME,STANDARD_CONFIG_SMOOTH_COLORING);
    currentConfigName=STANDARD_CONFIG_NAME;
    currentConfig=STANDARD_CONFIG;
    updateConfigUI();
    ui->nameComboBox->setCurrentText(STANDARD_CONFIG_NAME);
    ui->nameComboBox->setCurrentIndex(ui->nameComboBox->findText(STANDARD_CONFIG_NAME));
    applyConfig();
}

void MandelbrotMainWindow::addConfig(QString name,const MandelbrotConfig& config)
{
    if(configurations.find(name)==configurations.end())
        ui->nameComboBox->insertItem(std::distance(configurations.begin(),configurations.find(name)),name);
    configurations[name]=config;
}

MandelbrotMainWindow::~MandelbrotMainWindow()
{
    renderThread.terminate();
    scene.removeItem(&mandelbrotPixmapItem);
    delete ui;
}

void MandelbrotMainWindow::resizeTimerExpired()
{
    renderImage();
}

void MandelbrotMainWindow::updateImageOffsetDrag(QPoint newOffset)
{
    mandelbrotPixmapItem.setOffset(newOffset);
    ui->mandelbrotGraphicsView->update();
}

void MandelbrotMainWindow::updateImageOffsetRelease(QPoint newOffset)
{
    currentConfig.centerX-=currentConfig.scale*newOffset.rx();
    currentConfig.centerY-=currentConfig.scale*newOffset.ry();
    ui->xLineEdit->setText(QString::number(currentConfig.centerX));
    ui->yLineEdit->setText(QString::number(currentConfig.centerY));
    renderImage();
}

void MandelbrotMainWindow::readConfigs()
{
    QFile file("config.cfg");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    configurations.clear();
    QTextStream in(&file);
    ui->nameComboBox->clear();
    QString name;
    MandelbrotConfig config;
    while(!in.atEnd())
    {
        name=in.readLine();
        config.formula=in.readLine();
        config.limit=in.readLine().toDouble();
        config.centerX=in.readLine().toDouble();
        config.centerY=in.readLine().toDouble();
        config.scale=in.readLine().toDouble();
        config.nIterations=in.readLine().toInt();
        config.colorPaletteFileName=in.readLine();
        config.paletteFormulaX=in.readLine();
        config.col0interior=!!in.readLine().toInt();
        config.paletteFormulaY=in.readLine();
        config.row0interior=!!in.readLine().toInt();
        config.julia=!!in.readLine().toInt();
        config.juliaRe=in.readLine().toDouble();
        config.juliaIm=in.readLine().toDouble();
        if(configurations.find(name)==configurations.end())
        {
            configurations[name]=config;
            ui->nameComboBox->addItem(name);
        }
    }
}

void MandelbrotMainWindow::writeConfigs()
{
    QFile file("config.cfg");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    std::map<QString,MandelbrotConfig>::iterator it=configurations.begin();
    QTextStream out(&file);
    std::pair<QString,MandelbrotConfig> p;
    while(it!=configurations.end())
    {
        p=*it;
        out<<p.first<<"\n";
        out<<p.second.formula<<"\n";
        out<<QString::number(p.second.limit)<<"\n";
        out<<QString::number(p.second.centerX)<<"\n";
        out<<QString::number(p.second.centerY)<<"\n";
        out<<QString::number(p.second.scale)<<"\n";
        out<<QString::number(p.second.nIterations)<<"\n";
        out<<p.second.colorPaletteFileName<<"\n";
        out<<p.second.paletteFormulaX<<"\n";
        out<<QString::number((int)p.second.col0interior)<<"\n";
        out<<p.second.paletteFormulaY<<"\n";
        out<<QString::number((int)p.second.row0interior)<<"\n";
        out<<QString::number((int)p.second.julia)<<"\n";
        out<<QString::number(p.second.juliaRe)<<"\n";
        out<<QString::number(p.second.juliaIm)<<"\n";
        ++it;
    }
}

void MandelbrotMainWindow::updateImageViewRect(QRectF viewRect)
{
    currentConfig.centerX+=currentConfig.scale*(viewRect.x()+viewRect.width()/2-ui->mandelbrotGraphicsView->width()/2);
    currentConfig.centerY+=currentConfig.scale*(viewRect.y()+viewRect.height()/2-ui->mandelbrotGraphicsView->height()/2);
    currentConfig.scale*=viewRect.width()/ui->mandelbrotGraphicsView->width();
    ui->xLineEdit->setText(QString::number(currentConfig.centerX));
    ui->yLineEdit->setText(QString::number(currentConfig.centerY));
    ui->scaleLineEdit->setText(QString::number(currentConfig.scale));
    renderImage();
}

void MandelbrotMainWindow::updateConfigUI()
{
    ui->formulaLineEdit->setText(currentConfig.formula);
    ui->limitLineEdit->setText(QString::number(currentConfig.limit));
    ui->xLineEdit->setText(QString::number(currentConfig.centerX));
    ui->yLineEdit->setText(QString::number(currentConfig.centerY));
    ui->scaleLineEdit->setText(QString::number(currentConfig.scale));
    ui->iterationsLineEdit->setText(QString::number(currentConfig.nIterations));
    ui->paletteFormulaXLineEdit->setText(currentConfig.paletteFormulaX);
    ui->col0CheckBox->setChecked(currentConfig.col0interior);
    ui->paletteFormulaYLineEdit->setText(currentConfig.paletteFormulaY);
    ui->row0CheckBox->setChecked(currentConfig.row0interior);
    ui->mandelbrotRadioButton->setChecked(!currentConfig.julia);
    ui->juliaRadioButton->setChecked(currentConfig.julia);
    ui->juliaXLineEdit->setText(QString::number(currentConfig.juliaRe));
    ui->juliaYLineEdit->setText(QString::number(currentConfig.juliaIm));
    updateColorPalettePreview();
}

int MandelbrotMainWindow::setConfigToUIContents()
{
    int errorCode=0;
    bool ok;
    currentConfig.formula=ui->formulaLineEdit->text();
    currentConfig.limit=ui->limitLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.centerX=ui->xLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.centerY=ui->yLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.scale=ui->scaleLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.nIterations=ui->iterationsLineEdit->text().toInt(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.paletteFormulaX=ui->paletteFormulaXLineEdit->text();
    currentConfig.col0interior=ui->col0CheckBox->isChecked();
    currentConfig.paletteFormulaY=ui->paletteFormulaYLineEdit->text();
    currentConfig.row0interior=ui->row0CheckBox->isChecked();
    currentConfig.julia=!ui->mandelbrotRadioButton->isChecked();
    currentConfig.juliaRe=ui->juliaXLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.juliaIm=ui->juliaYLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    return errorCode;
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
        updateZoomRect(zoomClickPos,event->pos());
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

void MandelbrotMainWindow::updateImage(QImage image)
{
    pixmap=QPixmap::fromImage(image);
    mandelbrotPixmapItem.setPos(0,0);
    mandelbrotPixmapItem.setOffset(0,0);
    mandelbrotPixmapItem.setPixmap(pixmap);
    ui->mandelbrotGraphicsView->update();
}

void MandelbrotMainWindow::renderImage()
{
    if(!currentConfig.julia)
        renderMandelbrot();
    else
        renderJulia();
}

void MandelbrotMainWindow::renderMandelbrot()
{
    emit renderMandelbrot(currentConfig.centerX,currentConfig.centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),currentConfig.scale,currentConfig.nIterations,currentConfig.limit,2);
}
void MandelbrotMainWindow::renderJulia()
{
    emit renderJulia(currentConfig.centerX,currentConfig.centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),currentConfig.scale,currentConfig.nIterations,currentConfig.limit,2,currentConfig.juliaRe,currentConfig.juliaIm);
}

void MandelbrotMainWindow::generateDefaultPalette()
{
    defaultPalette=QImage(256,1,QImage::Format_RGB32);
    for(int i=0;i<256;++i)
      ((unsigned long*)defaultPalette.scanLine(0))[i]=(unsigned long)qRgb(i,i,0);
}

void MandelbrotMainWindow::updateColorPalettePreview()
{
    QImage colorPalette;
    if(currentConfig.colorPaletteFileName=="" || !colorPalette.load(currentConfig.colorPaletteFileName))
        colorPalette=defaultPalette;
    ui->colorPalettePreviewLabel->setPixmap(QPixmap::fromImage(colorPalette));
}

void MandelbrotMainWindow::on_setColorPalettePushButton_clicked()
{
    QString fileName=QFileDialog::getOpenFileName(0,"Select color palette");
    currentConfig.colorPaletteFileName=fileName;
    updateColorPalettePreview();
}


void MandelbrotMainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    scene.setSceneRect(0,0,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height());
    mandelbrotPixmapItem.setPos(ui->mandelbrotGraphicsView->width()/2-mandelbrotPixmapItem.pixmap().width()/2,ui->mandelbrotGraphicsView->height()/2-mandelbrotPixmapItem.pixmap().height()/2);
    resizeTimer.start(500);
}

void MandelbrotMainWindow::on_nameComboBox_activated(const QString &str)
{
    if(configurations.find(str)==configurations.end())
        return;
    else
    {
        currentConfigName=str;
        currentConfig=configurations[str];
    }
    updateConfigUI();
    applyConfig();
    renderImage();
}

void MandelbrotMainWindow::saveImage()
{
    QString fileName=QFileDialog::getSaveFileName(0,"Save image");
    if(fileName!="")
        mandelbrotPixmapItem.pixmap().save(fileName);
}

void MandelbrotMainWindow::saveConfig()
{
    currentConfigName=ui->nameComboBox->currentText();
    addConfig(currentConfigName,currentConfig);
    writeConfigs();
}

void MandelbrotMainWindow::on_saveConfigPushButton_clicked()
{
    setConfigToUIContents();
    saveConfig();
}

void MandelbrotMainWindow::on_saveImagePushButton_clicked()
{
    saveImage();
}

void MandelbrotMainWindow::restoreConfig()
{
    currentConfig=configurations[currentConfigName];
    updateConfigUI();
    ui->nameComboBox->setCurrentText(currentConfigName);
}

void MandelbrotMainWindow::on_restoreConfigPushButton_clicked()
{
    restoreConfig();
    applyConfig();
    renderImage();
}

void MandelbrotMainWindow::deleteConfig()
{
    if(currentConfigName!=STANDARD_CONFIG_NAME)
    {
        configurations.erase(currentConfigName);
        ui->nameComboBox->removeItem(ui->nameComboBox->currentIndex());
        currentConfigName=STANDARD_CONFIG_NAME;
        currentConfig=STANDARD_CONFIG;
        updateConfigUI();
        ui->nameComboBox->setCurrentText(currentConfigName);
        ui->nameComboBox->setCurrentIndex(ui->nameComboBox->findText(currentConfigName));
        writeConfigs();
    }
}

void MandelbrotMainWindow::on_deleteConfigPushButton_clicked()
{
    deleteConfig();
    applyConfig();
    renderImage();
}

void MandelbrotMainWindow::applyConfig()
{
    emit parseFormula(currentConfig.formula);
    QImage colorPalette;
    if(currentConfig.colorPaletteFileName=="" || !colorPalette.load(currentConfig.colorPaletteFileName))
        colorPalette=defaultPalette;
    emit setColorPalette(colorPalette);
    emit parsePaletteXFormula(currentConfig.paletteFormulaX);
    emit parsePaletteYFormula(currentConfig.paletteFormulaY);
    emit setCol0Interior(currentConfig.col0interior);
    emit setRow0Interior(currentConfig.row0interior);
}

void MandelbrotMainWindow::on_applyPushButton_clicked()
{
    setConfigToUIContents();
    applyConfig();
    renderImage();
}

void MandelbrotMainWindow::on_mandelbrotRadioButton_toggled(bool checked)
{
    currentConfig.julia=!checked;
    ui->juliaXLabel->setVisible(!checked);
    ui->juliaXLineEdit->setVisible(!checked);
    ui->juliaYLabel->setVisible(!checked);
    ui->juliaYLineEdit->setVisible(!checked);
}

void MandelbrotMainWindow::receiveErrorCode(int errorCode)
{

}
