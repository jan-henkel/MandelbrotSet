#include "mandelbrotmainwindow.h"
#include "ui_mandelbrotmainwindow.h"
#include <QFile>
#include <QTextStream>

MandelbrotMainWindow::MandelbrotMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MandelbrotMainWindow),
    STANDARD_CONFIG_NAME("Standard Mandelbrot"),
    STANDARD_CONFIG({"z^2+c",       //formula
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
                    })
{
    mandelbrotSet.moveToThread(&renderThread);
    renderThread.start();
    ui->setupUi(this);
    mandelbrotPixmapItem.setPixmap(pixmap);
    scene.setSceneRect(0,0,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height());
    ui->mandelbrotGraphicsView->setScene(&scene);
    scene.addItem(&mandelbrotPixmapItem);
    QObject::connect(&mandelbrotSet,SIGNAL(imageOut(QImage)),this,SLOT(updateImage(QImage)),Qt::QueuedConnection);
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateOffsetDrag(QPoint)),this,SLOT(updateImageOffsetDrag(QPoint)));
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateOffsetRelease(QPoint)),this,SLOT(updateImageOffsetRelease(QPoint)));
    QObject::connect(ui->mandelbrotGraphicsView,SIGNAL(updateViewRect(QRectF)),this,SLOT(updateImageViewRect(QRectF)));
    QObject::connect(this,SIGNAL(renderMandelbrot(double,double,int,int,double,int,double,int)),&mandelbrotSet,SLOT(renderMandelbrot(double,double,int,int,double,int,double,int)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(renderJulia(double,double,int,int,double,int,double,int,double,double)),&mandelbrotSet,SLOT(renderJulia(double,double,int,int,double,int,double,int,double,double)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(parseFormula(QString)),&mandelbrotSet,SLOT(parseFormula(QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(parsePaletteXFormula(QString)),&mandelbrotSet,SLOT(parsePaletteXFormula(QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(parsePaletteYFormula(QString)),&mandelbrotSet,SLOT(parsePaletteYFormula(QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(setCol0Interior(bool)),&mandelbrotSet,SLOT(setCol0Interior(bool)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(setRow0Interior(bool)),&mandelbrotSet,SLOT(setRow0Interior(bool)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(setColorPalette(QImage)),&mandelbrotSet,SLOT(setColorPalette(QImage)),Qt::QueuedConnection);
    readConfigs();
    currentConfig.first=STANDARD_CONFIG_NAME;
    currentConfig.second=STANDARD_CONFIG;
    configurations[STANDARD_CONFIG_NAME]=STANDARD_CONFIG;
    updateConfigUI();
    ui->nameComboBox->setCurrentText(STANDARD_CONFIG_NAME);
}

MandelbrotMainWindow::~MandelbrotMainWindow()
{
    renderThread.terminate();
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
    currentConfig.second.centerX-=currentConfig.second.scale*newOffset.rx();
    currentConfig.second.centerY-=currentConfig.second.scale*newOffset.ry();
    updateConfigUI();
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
    ui->nameComboBox->setInsertPolicy(QComboBox::InsertAlphabetically);
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
        configurations.insert(std::pair<QString,MandelbrotConfig>(name,config));
        ui->nameComboBox->addItem(name);
    }
    ui->nameComboBox->setInsertPolicy(QComboBox::NoInsert);
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
    currentConfig.second.centerX+=currentConfig.second.scale*(viewRect.x()+viewRect.width()/2-ui->mandelbrotGraphicsView->width()/2);
    currentConfig.second.centerY+=currentConfig.second.scale*(viewRect.y()+viewRect.height()/2-ui->mandelbrotGraphicsView->height()/2);
    currentConfig.second.scale*=viewRect.width()/ui->mandelbrotGraphicsView->width();
    updateConfigUI();
    renderImage();
}

void MandelbrotMainWindow::updateConfigUI()
{
    ui->formulaLineEdit->setText(currentConfig.second.formula);
    ui->limitLineEdit->setText(QString::number(currentConfig.second.limit));
    ui->xLineEdit->setText(QString::number(currentConfig.second.centerX));
    ui->yLineEdit->setText(QString::number(currentConfig.second.centerY));
    ui->scaleLineEdit->setText(QString::number(currentConfig.second.scale));
    ui->iterationsLineEdit->setText(QString::number(currentConfig.second.nIterations));
    ui->paletteFormulaXLineEdit->setText(currentConfig.second.paletteFormulaX);
    ui->col0CheckBox->setChecked(currentConfig.second.col0interior);
    ui->paletteFormulaYLineEdit->setText(currentConfig.second.paletteFormulaY);
    ui->row0CheckBox->setChecked(currentConfig.second.row0interior);
    ui->mandelbrotRadioButton->setChecked(!currentConfig.second.julia);
    ui->juliaRadioButton->setChecked(currentConfig.second.julia);
    ui->juliaXLineEdit->setText(QString::number(currentConfig.second.juliaRe));
    ui->juliaYLineEdit->setText(QString::number(currentConfig.second.juliaIm));
}

int MandelbrotMainWindow::setConfigToUIContents()
{
    int errorCode=0;
    bool ok;
    currentConfig.second.formula=ui->formulaLineEdit->text();
    currentConfig.second.limit=ui->limitLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.second.centerX=ui->xLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.second.centerY=ui->yLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.second.scale=ui->scaleLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.second.nIterations=ui->iterationsLineEdit->text().toInt(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.second.paletteFormulaX=ui->paletteFormulaXLineEdit->text();
    currentConfig.second.col0interior=ui->col0CheckBox->isChecked();
    currentConfig.second.paletteFormulaY=ui->paletteFormulaYLineEdit->text();
    currentConfig.second.row0interior=ui->row0CheckBox->isChecked();
    currentConfig.second.julia=!ui->mandelbrotRadioButton->isChecked();
    currentConfig.second.juliaRe=ui->juliaXLineEdit->text().toDouble(&ok);
    errorCode<<=1;
    errorCode|=(int)ok;
    currentConfig.second.juliaIm=ui->juliaYLineEdit->text().toDouble(&ok);
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
    if(!currentConfig.second.julia)
        renderMandelbrot();
    else
        renderJulia();
}

void MandelbrotMainWindow::renderMandelbrot()
{
    emit renderMandelbrot(currentConfig.second.centerX,currentConfig.second.centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),currentConfig.second.scale,currentConfig.second.nIterations,currentConfig.second.limit,2);
}
void MandelbrotMainWindow::renderJulia()
{
    emit renderJulia(currentConfig.second.centerX,currentConfig.second.centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),currentConfig.second.scale,currentConfig.second.nIterations,currentConfig.second.limit,2,currentConfig.second.juliaRe,currentConfig.second.juliaIm);
}

void MandelbrotMainWindow::selectColorPalette()
{
    QImage palette;
    QString fileName=QFileDialog::getOpenFileName(0,"Select color palette");
    if(fileName!="" && palette.load(fileName))
    {
        emit setColorPalette(palette);
    }
}

void MandelbrotMainWindow::on_setColorPalettePushButton_clicked()
{
    selectColorPalette();
}


void MandelbrotMainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    scene.setSceneRect(0,0,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height());
    mandelbrotPixmapItem.setPos(ui->mandelbrotGraphicsView->width()/2-mandelbrotPixmapItem.pixmap().width()/2,ui->mandelbrotGraphicsView->height()/2-mandelbrotPixmapItem.pixmap().height()/2);
    renderMandelbrot();
}

void MandelbrotMainWindow::on_nameComboBox_activated(const QString &str)
{
    if(configurations.find(str)==configurations.end())
        return;
    else
    {
        currentConfig.first=str;
        currentConfig.second=configurations[str];
    }
    updateConfigUI();
}

void MandelbrotMainWindow::saveImage()
{
    QString fileName=QFileDialog::getSaveFileName(0,"Save image");
    if(fileName!="")
        mandelbrotPixmapItem.pixmap().save(fileName);
}

void MandelbrotMainWindow::saveConfig()
{
    setConfigToUIContents();
    currentConfig.first=ui->nameComboBox->currentText();
    if(configurations.find(currentConfig.first)==configurations.end())
    {
        ui->nameComboBox->setInsertPolicy(QComboBox::InsertAlphabetically);
        ui->nameComboBox->addItem(currentConfig.first);
        ui->nameComboBox->setInsertPolicy(QComboBox::NoInsert);
    }
    configurations[currentConfig.first]=currentConfig.second;
    writeConfigs();
}

void MandelbrotMainWindow::on_saveConfigPushButton_clicked()
{
    saveConfig();
}

void MandelbrotMainWindow::on_saveImagePushButton_clicked()
{
    saveImage();
}

void MandelbrotMainWindow::restoreConfig()
{
    currentConfig.second=configurations[currentConfig.first];
    updateConfigUI();
    ui->nameComboBox->setCurrentText(currentConfig.first);
}

void MandelbrotMainWindow::on_restoreConfigPushButton_clicked()
{
    restoreConfig();
}

void MandelbrotMainWindow::deleteConfig()
{
    configurations.erase(currentConfig.first);
    ui->nameComboBox->removeItem(ui->nameComboBox->currentIndex());
    currentConfig.first=STANDARD_CONFIG_NAME;
    currentConfig.second=STANDARD_CONFIG;
    updateConfigUI();
    ui->nameComboBox->setCurrentText(currentConfig.first);
    writeConfigs();
}

void MandelbrotMainWindow::on_deleteConfigPushButton_clicked()
{
    deleteConfig();
}

void MandelbrotMainWindow::applyConfig()
{
    setConfigToUIContents();
    emit parseFormula(currentConfig.second.formula);
    emit parsePaletteXFormula(currentConfig.second.paletteFormulaX);
    emit parsePaletteYFormula(currentConfig.second.paletteFormulaY);
    emit setCol0Interior(currentConfig.second.col0interior);
    emit setRow0Interior(currentConfig.second.row0interior);
    renderImage();
}

void MandelbrotMainWindow::on_applyPushButton_clicked()
{
    applyConfig();
}

void MandelbrotMainWindow::on_mandelbrotRadioButton_toggled(bool checked)
{
    currentConfig.second.julia=!checked;
    ui->juliaXLabel->setVisible(!checked);
    ui->juliaXLineEdit->setVisible(!checked);
    ui->juliaYLabel->setVisible(!checked);
    ui->juliaYLineEdit->setVisible(!checked);
}

void MandelbrotMainWindow::receiveErrorCode(int errorCode)
{

}
