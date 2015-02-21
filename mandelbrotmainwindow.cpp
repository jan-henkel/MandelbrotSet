#include "mandelbrotmainwindow.h"
#include "ui_mandelbrotmainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QToolTip>
#include <QMimeData>

//definitions of default configs
const QString MandelbrotMainWindow::DEFAULT_CONFIG_NAME="Standard Mandelbrot";
const MandelbrotConfig MandelbrotMainWindow::DEFAULT_CONFIG=
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
const QString MandelbrotMainWindow::DEFAULT_CONFIG_SMOOTH_COLORING_NAME="Standard Mandelbrot (smooth coloring)";
const MandelbrotConfig MandelbrotMainWindow::DEFAULT_CONFIG_SMOOTH_COLORING=
        {"z^2+c",                                           //formula
        20.0,                                                //limit
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

const int MandelbrotMainWindow::MIN_ZOOM_WIDTH=20;
const int MandelbrotMainWindow::MIN_ZOOM_HEIGHT=20;
const int MandelbrotMainWindow::MIN_DRAG_DISTANCE_SQUARED=16;

const int MandelbrotMainWindow::DEFAULT_ITERATIONS=100;
const double MandelbrotMainWindow::DEFAULT_SCALE=0.007;
const double MandelbrotMainWindow::DEFAULT_LIMIT=100.;

const int MandelbrotMainWindow::PASSES=2;

MandelbrotMainWindow::MandelbrotMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MandelbrotMainWindow)
{
    //set up multithreading
    mandelbrotSet.moveToThread(&workerThread);
    workerThread.start();

    //set up UI and render area
    ui->setupUi(this);
    ui->colorPalettePreviewLabel->setAcceptDrops(true);
    ui->colorPalettePreviewLabel->setMouseTracking(true);
    ui->colorPalettePreviewLabel->installEventFilter(this);
    mandelbrotPixmapItem.setPixmap(mandelbrotPixmap);
    mandelbrotScene.setSceneRect(0,0,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height());
    ui->mandelbrotGraphicsView->setScene(&mandelbrotScene);
    mandelbrotScene.addItem(&mandelbrotPixmapItem);
    ui->mandelbrotGraphicsView->setMouseTracking(true);
    ui->mandelbrotGraphicsView->installEventFilter(this);
    ui->mandelbrotGraphicsView->viewport()->installEventFilter(this);
    ui->renderProgressLabel->setVisible(false);
    ui->renderProgressBar->setVisible(false);

    //set up communication between mandelbrotSet object and this window
    QObject::connect(&mandelbrotSet,SIGNAL(imageOut(QImage)),this,SLOT(updateImage(QImage)),Qt::QueuedConnection);
    QObject::connect(&mandelbrotSet,SIGNAL(errorCodeOut(int)),this,SLOT(receiveErrorCode(int)),Qt::QueuedConnection);
    QObject::connect(&mandelbrotSet,SIGNAL(linesRendered(int)),ui->renderProgressBar,SLOT(setValue(int)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(renderMandelbrot(double,double,int,int,double,int,double,int)),&mandelbrotSet,SLOT(renderMandelbrot(double,double,int,int,double,int,double,int)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(renderJulia(double,double,int,int,double,int,double,int,double,double)),&mandelbrotSet,SLOT(renderJulia(double,double,int,int,double,int,double,int,double,double)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(parseFormula(QString)),&mandelbrotSet,SLOT(parseFormula(QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(parsePaletteXFormula(QString)),&mandelbrotSet,SLOT(parsePaletteXFormula(QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(parsePaletteYFormula(QString)),&mandelbrotSet,SLOT(parsePaletteYFormula(QString)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(setCol0Interior(bool)),&mandelbrotSet,SLOT(setCol0Interior(bool)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(setRow0Interior(bool)),&mandelbrotSet,SLOT(setRow0Interior(bool)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(setColorPalette(QImage)),&mandelbrotSet,SLOT(setColorPalette(QImage)),Qt::QueuedConnection);

    //set up delayedRenderTimer
    delayedRenderTimer.setSingleShot(true);
    QObject::connect(&delayedRenderTimer,SIGNAL(timeout()),this,SLOT(renderImage()));

    //generate default color palette
    generateDefaultPalette();

    //read configurations from config.cfg, set up default configurations
    readConfigs();
    addConfig(DEFAULT_CONFIG_NAME,DEFAULT_CONFIG);
    addConfig(DEFAULT_CONFIG_SMOOTH_COLORING_NAME,DEFAULT_CONFIG_SMOOTH_COLORING);
    currentConfigName=DEFAULT_CONFIG_NAME;
    currentConfig=DEFAULT_CONFIG;
    ui->nameComboBox->setCurrentText(DEFAULT_CONFIG_NAME);
    ui->nameComboBox->setCurrentIndex(ui->nameComboBox->findText(DEFAULT_CONFIG_NAME));
    applyCurrentConfig();
    updateConfigUI();
}

MandelbrotMainWindow::~MandelbrotMainWindow()
{
    //cleanup
    workerThread.terminate();
    mandelbrotScene.removeItem(&mandelbrotPixmapItem);
    delete ui;
}

void MandelbrotMainWindow::renderImage()
{
    //show render progress bar, set upper limit to number of lines to render
    ui->renderProgressLabel->setVisible(true);
    ui->renderProgressBar->setVisible(true);
    ui->renderProgressBar->setRange(0,ui->mandelbrotGraphicsView->height()*PASSES);
    ui->renderProgressBar->setValue(0);

    //cancel ongoing render
    mandelbrotSet.cancel();

    //render Mandelbrot- or Julia-type images depending on current configuration
    if(!currentConfig.julia)
        emit renderMandelbrot(currentConfig.centerX,currentConfig.centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),currentConfig.scale,currentConfig.nIterations,currentConfig.limit,PASSES);
    else
        emit renderJulia(currentConfig.centerX,currentConfig.centerY,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height(),currentConfig.scale,currentConfig.nIterations,currentConfig.limit,PASSES,currentConfig.juliaRe,currentConfig.juliaIm);
}

/*
 *
 *
 *
 * processing of signals from worker thread
 *
 *
 *
 */

void MandelbrotMainWindow::updateImage(QImage image)
{
    mandelbrotPixmap=QPixmap::fromImage(image);
    mandelbrotPixmapItem.setPos(0,0);
    mandelbrotPixmapItem.setOffset(0,0);
    preDragOffset=QPoint(0,0);
    mandelbrotPixmapItem.setPixmap(mandelbrotPixmap);
    ui->mandelbrotGraphicsView->update();
}

void MandelbrotMainWindow::receiveErrorCode(int errorCode)
{
    //show errors in status bar
    QString message;
    if(!errorCode)
        message="No errors.";
    else
    {
        ui->renderProgressLabel->setVisible(false);
        ui->renderProgressBar->setVisible(false);
        message="";
        if(errorCode&MandelbrotSet::FORMULA_PARSE_ERROR)
            message+="Error parsing formula. ";
        if(errorCode&MandelbrotSet::PALETTE_XFORMULA_PARSE_ERROR)
            message+="Error parsing coloring formula, x-coordinate. ";
        if(errorCode&MandelbrotSet::PALETTE_YFORMULA_PARSE_ERROR)
            message+="Error parsing coloring formula, y-coordinate. ";
    }
    ui->statusBar->showMessage(message,5000);
}

/*
 *
 *
 *
 *  event handlers and input processing
 *
 *
 *
 */

//render area input processing
bool MandelbrotMainWindow::eventFilter(QObject *target, QEvent *e)
{
    if(target==ui->mandelbrotGraphicsView->viewport() || target==ui->mandelbrotGraphicsView)
    {
        switch(e->type())
        {
        case QEvent::MouseButtonPress:
        {
            QMouseEvent* event=(QMouseEvent*)e;
            if(event->button()==Qt::LeftButton)
            {
                //remember click position, add zoom rectangle of minimal size to render area
                zoomClickPos=event->pos();
                int w,h,renderWidth,renderHeight;
                w=MIN_ZOOM_WIDTH;
                h=MIN_ZOOM_HEIGHT;
                renderWidth=ui->mandelbrotGraphicsView->width();
                renderHeight=ui->mandelbrotGraphicsView->height();
                if(renderWidth>renderHeight)
                    w=(h*renderWidth)/renderHeight;
                else
                    h=(w*renderHeight)/renderWidth;
                zoomRect.setRect(event->pos().x(),event->pos().y(),w,h);
                //set zoomRect color to black or white to achieve best contrast with current palette
                QColor color=QColor(currentColorPalette.pixel(0,0));
                color=((color.red()+color.green()+color.blue())>3*128)?QColor(0,0,0):QColor(255,255,255);
                zoomRect.setPen(QPen(color));
                mandelbrotScene.addItem(&zoomRect);
                ui->mandelbrotGraphicsView->update();
                return true;
            }
            else if(event->button()==Qt::RightButton)
            {
                //remember click position
                dragClickPos=event->pos();
                preDragOffset=mandelbrotPixmapItem.offset().toPoint();
                return true;
            }
            break;
        }
        case QEvent::MouseMove:
        {
            QMouseEvent* event=(QMouseEvent*)e;
            if(event->buttons() & Qt::LeftButton)
            {
                //update zoom rectangle
                QPoint d=((QMouseEvent*)e)->pos()-zoomClickPos;
                int w,h,renderWidth,renderHeight;
                renderWidth=ui->mandelbrotGraphicsView->width();
                renderHeight=ui->mandelbrotGraphicsView->height();
                w=(d.x()<MIN_ZOOM_WIDTH)?MIN_ZOOM_WIDTH:d.x();
                h=(d.y()<MIN_ZOOM_HEIGHT)?MIN_ZOOM_HEIGHT:d.y();
                w=((h*renderWidth)/renderHeight<=w)?w:(h*renderWidth)/renderHeight;
                h=((w*renderHeight)/renderWidth<=h)?h:(w*renderHeight)/renderWidth;
                zoomRect.setRect(zoomClickPos.x(),zoomClickPos.y(),w,h);
                this->update();
                return true;
            }
            if(event->buttons() & Qt::RightButton)
            {
                QPoint offset=event->pos()-dragClickPos;
                if(offset.x()*offset.x()+offset.y()*offset.y()>=MIN_DRAG_DISTANCE_SQUARED)
                {
                    //drag image to new position on render area, don't rerender yet
                    mandelbrotPixmapItem.setOffset(event->pos()-dragClickPos+preDragOffset);
                    ui->mandelbrotGraphicsView->update();
                    return true;
                }
                else
                    mandelbrotPixmapItem.setOffset(preDragOffset);
            }
            QPoint p=QPoint(ui->mandelbrotGraphicsView->width()/2,ui->mandelbrotGraphicsView->height()/2);
            p=event->pos()-p;
            ui->statusBar->showMessage("("+QString::number(currentConfig.scale*p.x()+currentConfig.centerX)+","+QString::number(currentConfig.scale*p.y()+currentConfig.centerY)+")",5000);
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent* event=(QMouseEvent*)e;
            if(event->button() == Qt::LeftButton)
            {
                //remove zoomRect from render area
                mandelbrotScene.removeItem(&zoomRect);
                ui->mandelbrotGraphicsView->update();
                //zoom in according to zoomRect, rerender
                QRectF rect=zoomRect.rect();
                zoomToRect(rect);
                return true;
            }
            else if(event->button() == Qt::RightButton)
            {
                QPoint offset=event->pos()-dragClickPos;
                if(offset.x()*offset.x()+offset.y()*offset.y()>=MIN_DRAG_DISTANCE_SQUARED)
                {
                    //rerender at new position
                    moveByOffset(offset);
                    return true;
                }
                else
                {
                    //context menu
                    QMenu menu;
                    QAction* renderJuliaAction=menu.addAction("Explore Julia set");
                    QAction* centerAction=menu.addAction("Center on this point");
                    menu.move(ui->mandelbrotGraphicsView->viewport()->mapToGlobal(event->pos()));
                    QAction* result=menu.exec();
                    if(result==renderJuliaAction)
                    {
                        //render Julia set centered on 0 with c=(juliaRe,juliaIm) set to this point
                        QPoint p=QPoint(ui->mandelbrotGraphicsView->width()/2,ui->mandelbrotGraphicsView->height()/2);
                        p=event->pos()-p;
                        currentConfig.julia=true;
                        //order of assignments is important, since centerX, centerY, scale will be changed
                        currentConfig.juliaIm=currentConfig.centerY+currentConfig.scale*p.y();
                        currentConfig.juliaRe=currentConfig.centerX+currentConfig.scale*p.x();
                        currentConfig.centerX=0;
                        currentConfig.centerY=0;
                        currentConfig.limit=DEFAULT_LIMIT;
                        currentConfig.nIterations=DEFAULT_ITERATIONS;
                        currentConfig.scale=DEFAULT_SCALE;
                        updateConfigUI();
                        applyCurrentConfig();
                        renderImage();
                        ui->applyPushButton->setEnabled(false);
                        return true;
                    }
                    else if(result==centerAction)
                    {
                        QPoint p=QPoint(ui->mandelbrotGraphicsView->width()/2,ui->mandelbrotGraphicsView->height()/2);
                        p=p-event->pos();
                        moveByOffset(p);
                        return true;
                    }
                    else
                    {
                        return true;
                    }
                }
            }
            break;
        }
        case QEvent::Wheel:
        {
            //zoom in and out
            QWheelEvent* event=(QWheelEvent*)e;
            currentConfig.scale*=pow(16.,-(double)event->angleDelta().y()/(8.*360.));
            ui->scaleLineEdit->setText(QString::number(currentConfig.scale));
            delayedRenderTimer.start(200);
            return false;
            break;
        }
        case QEvent::KeyRelease:
        {
            //zoom in and out
            QKeyEvent* event=(QKeyEvent*)e;
            if(event->key()==Qt::Key_Plus)
            {
                currentConfig.scale/=2.;
                ui->scaleLineEdit->setText(QString::number(currentConfig.scale));
                renderImage();
            }
            else if(event->key()==Qt::Key_Minus)
            {
                currentConfig.scale*=2.;
                ui->scaleLineEdit->setText(QString::number(currentConfig.scale));
                renderImage();
            }
            break;
        }
        default:
            return false;
            break;
        }
    }
    else if(target==ui->colorPalettePreviewLabel)
    {
        switch(e->type())
        {
        //drag-drop color palette into preview label
        case QEvent::DragEnter:
        {
            QDragEnterEvent* event=(QDragEnterEvent*)e;
            if(event->mimeData()->hasUrls())
                event->accept();
            return true;
            break;
        }
        case QEvent::Drop:
        {
            QDropEvent* event=(QDropEvent*)e;
            if(event->mimeData()->hasUrls())
            {
                event->accept();
                currentConfig.colorPaletteFileName=QDir(".").relativeFilePath(event->mimeData()->urls().first().toLocalFile());
                updateColorPalettePreview();
                ui->applyPushButton->setEnabled(true);
            }
            return true;
            break;
        }
        //display X and Y coordinates of the color palette corresponding to the current mouse position
        case QEvent::MouseMove:
        {
            QMouseEvent* event=(QMouseEvent*)e;
            int w1=ui->colorPalettePreviewLabel->pixmap()->width(),w2=ui->colorPalettePreviewLabel->width();
            int h1=ui->colorPalettePreviewLabel->pixmap()->height(),h2=ui->colorPalettePreviewLabel->height();
            int offset=ui->colorPalettePreviewLabel->frameWidth();
            ui->statusBar->showMessage("X="+QString::number(((event->pos().x()-offset)*w1)/w2)+" Y="+QString::number(((event->pos().y()-offset)*h1)/h2),5000);
            break;
        }
        default:
            return false;
            break;
        }
    }
    return false;
}

//resize event handler
void MandelbrotMainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    mandelbrotScene.setSceneRect(0,0,ui->mandelbrotGraphicsView->width(),ui->mandelbrotGraphicsView->height());
    mandelbrotPixmapItem.setPos(ui->mandelbrotGraphicsView->width()/2-mandelbrotPixmapItem.pixmap().width()/2,ui->mandelbrotGraphicsView->height()/2-mandelbrotPixmapItem.pixmap().height()/2);
    delayedRenderTimer.start(300);
}

void MandelbrotMainWindow::zoomToRect(QRectF rect)
{
    //change current config according to zoom rectangle
    currentConfig.centerX+=currentConfig.scale*(rect.x()+rect.width()/2-ui->mandelbrotGraphicsView->width()/2);
    currentConfig.centerY+=currentConfig.scale*(rect.y()+rect.height()/2-ui->mandelbrotGraphicsView->height()/2);
    currentConfig.scale*=rect.width()/ui->mandelbrotGraphicsView->width();
    //update UI
    ui->xLineEdit->setText(QString::number(currentConfig.centerX));
    ui->yLineEdit->setText(QString::number(currentConfig.centerY));
    ui->scaleLineEdit->setText(QString::number(currentConfig.scale));
    //render selected area
    renderImage();
}

void MandelbrotMainWindow::moveByOffset(QPoint offset)
{
    //change current config according to offset
    currentConfig.centerX-=currentConfig.scale*offset.x();
    currentConfig.centerY-=currentConfig.scale*offset.y();
    //update UI
    ui->xLineEdit->setText(QString::number(currentConfig.centerX));
    ui->yLineEdit->setText(QString::number(currentConfig.centerY));
    //render new area
    renderImage();
}

/*
 *
 *
 *
 * UI input processing
 *
 *
 *
 */


void MandelbrotMainWindow::on_nameComboBox_activated(const QString &str)
{
    //user picked a new config from the combo box
    if(configurations.find(str)==configurations.end())
        return;
    else
    {
        currentConfigName=str;
        currentConfig=configurations[str];
    }
    updateConfigUI();
    applyCurrentConfig();
    renderImage();
    ui->applyPushButton->setEnabled(false);
}

void MandelbrotMainWindow::on_saveConfigPushButton_clicked()
{
    //user intends to save current settings
    setConfigToUIContents();
    saveCurrentConfig();
    ui->nameComboBox->setCurrentIndex(ui->nameComboBox->findText(currentConfigName));
}

void MandelbrotMainWindow::on_restoreConfigPushButton_clicked()
{
    //user wants to restore the config currently selected in the combo box
    restoreCurrentConfig();
    applyCurrentConfig();
    renderImage();
    ui->applyPushButton->setEnabled(false);
}

void MandelbrotMainWindow::on_deleteConfigPushButton_clicked()
{
    //user wants to delete the config currently selected in the combo box
    deleteCurrentConfig();
    applyCurrentConfig();
    renderImage();
    ui->applyPushButton->setEnabled(false);
}

void MandelbrotMainWindow::on_setColorPalettePushButton_clicked()
{
    //user selects new color palette from image
    QString fileName=QFileDialog::getOpenFileName(0,"Select color palette","./palettes","Image files (*.jpg *.png *.bmp)");
    currentConfig.colorPaletteFileName=QDir(".").relativeFilePath(fileName);
    updateColorPalettePreview();
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_mandelbrotRadioButton_toggled(bool checked)
{
    //user changes mode to Mandelbrot- or Julia-type images, modify UI accordingly
    ui->juliaXLabel->setVisible(!checked);
    ui->juliaXLineEdit->setVisible(!checked);
    ui->juliaYLabel->setVisible(!checked);
    ui->juliaYLineEdit->setVisible(!checked);
}

void MandelbrotMainWindow::on_applyPushButton_clicked()
{
    //user wants to apply the config as set and displayed on the UI
    setConfigToUIContents();
    applyCurrentConfig();
    renderImage();
    ui->applyPushButton->setEnabled(false);
}

void MandelbrotMainWindow::on_saveImagePushButton_clicked()
{
    //user wants to save rendered image to a file
    QString fileName=QFileDialog::getSaveFileName(0,"Save image","./images","Image files (*.jpg *.png *.bmp)");
    if(fileName!="")
        mandelbrotPixmapItem.pixmap().save(fileName);
}


/*
 *
 *
 *
 * config management functions
 *
 *
 *
 */

void MandelbrotMainWindow::addConfig(QString name,const MandelbrotConfig& config)
{
    //add config of the name specified. if it already exists, replace it
    if(configurations.find(name)==configurations.end())
    {
        configurations[name]=config;
        ui->nameComboBox->insertItem(std::distance(configurations.begin(),configurations.find(name)),name);
    }
    else
        configurations[name]=config;
}

void MandelbrotMainWindow::applyCurrentConfig()
{
    //update mandelbrotSet object according to current config
    emit parseFormula(currentConfig.formula);
    if(currentConfig.colorPaletteFileName=="" || !currentColorPalette.load(currentConfig.colorPaletteFileName))
        currentColorPalette=defaultPalette;
    ui->mandelbrotGraphicsView->setBackgroundBrush(QBrush(QColor(currentColorPalette.pixel(0,0))));
    emit setColorPalette(currentColorPalette);
    emit parsePaletteXFormula(currentConfig.paletteFormulaX);
    emit parsePaletteYFormula(currentConfig.paletteFormulaY);
    emit setCol0Interior(currentConfig.col0interior);
    emit setRow0Interior(currentConfig.row0interior);
}

void MandelbrotMainWindow::saveCurrentConfig()
{
    //add config (as currently displayed) to the configurations set and combo box and write the config list to config.cfg
    currentConfigName=ui->nameComboBox->currentText();
    addConfig(currentConfigName,currentConfig);
    writeConfigs();
}

void MandelbrotMainWindow::restoreCurrentConfig()
{
    //restore config settings from the config set
    currentConfig=configurations[currentConfigName];
    updateConfigUI();
    ui->nameComboBox->setCurrentText(currentConfigName);
}

void MandelbrotMainWindow::deleteCurrentConfig()
{
    //delete currently selected config from the config set (unless default config is selected
    if(currentConfigName!=DEFAULT_CONFIG_NAME && currentConfigName!=DEFAULT_CONFIG_SMOOTH_COLORING_NAME)
    {
        configurations.erase(currentConfigName);
        ui->nameComboBox->removeItem(ui->nameComboBox->currentIndex());
        currentConfigName=DEFAULT_CONFIG_NAME;
        currentConfig=DEFAULT_CONFIG;
        updateConfigUI();
        ui->nameComboBox->setCurrentText(currentConfigName);
        ui->nameComboBox->setCurrentIndex(ui->nameComboBox->findText(currentConfigName));
        writeConfigs();
    }
}


void MandelbrotMainWindow::readConfigs()
{
    //read config set from config.cfg, fill combo box with configurations
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
    //write config set to config.cfg
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

void MandelbrotMainWindow::updateConfigUI()
{
    //update UI to reflect current config
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

void MandelbrotMainWindow::updateColorPalettePreview()
{
    QImage palette;
    if(currentConfig.colorPaletteFileName=="" || !palette.load(currentConfig.colorPaletteFileName))
        palette=defaultPalette;
    ui->colorPalettePreviewLabel->setPixmap(QPixmap::fromImage(palette));
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

void MandelbrotMainWindow::generateDefaultPalette()
{
    static const int width=256;
    static const int ncolors=8;
    defaultPalette=QImage(width,1,QImage::Format_RGB32);
    QColor colors[]=
    {
        QColor(0,0,100),
        QColor(0,0,255),
        QColor(255,255,255),
        QColor(0,0,255),
        QColor(255,255,255),
        QColor(240,70,0),
        QColor(255,255,70),
        QColor(255,255,255)
    };
    ((unsigned long*)defaultPalette.scanLine(0))[0]=(unsigned long)qRgb(0,0,0);
    double seglength=(double)width/(double)(ncolors-1);
    int seg;
    double pos;
    for(int i=1;i<width;++i)
    {
        seg=i*(ncolors-1)/width;
        pos=((double)i-seg*seglength)/seglength;
        ((unsigned long*)defaultPalette.scanLine(0))[i]=(unsigned long)qRgb(
                colors[seg].red()*(1-pos)+colors[seg+1].red()*pos,
                colors[seg].green()*(1-pos)+colors[seg+1].green()*pos,
                colors[seg].blue()*(1-pos)+colors[seg+1].blue()*pos
                    );

    }
    defaultPalette.save("./palettes/default.jpg");
}

void MandelbrotMainWindow::on_formulaLineEdit_textEdited(const QString &)
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_limitLineEdit_textEdited(const QString &)
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_xLineEdit_textEdited(const QString &)
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_yLineEdit_textEdited(const QString &)
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_scaleLineEdit_textEdited(const QString &)
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_iterationsLineEdit_textEdited(const QString &)
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_paletteFormulaXLineEdit_textEdited(const QString &)
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_col0CheckBox_clicked()
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_paletteFormulaYLineEdit_textEdited(const QString &)
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_row0CheckBox_clicked()
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_mandelbrotRadioButton_clicked()
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_juliaRadioButton_clicked()
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_juliaXLineEdit_textEdited(const QString &)
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_juliaYLineEdit_textEdited(const QString &)
{
    ui->applyPushButton->setEnabled(true);
}

void MandelbrotMainWindow::on_renderProgressBar_valueChanged(int value)
{
    if(value==ui->renderProgressBar->maximum())
    {
        ui->renderProgressLabel->setVisible(false);
        ui->renderProgressBar->setVisible(false);
    }
}
