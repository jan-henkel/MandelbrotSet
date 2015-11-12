#include "mandelbrotmainwindow.h"
#include <QApplication>

int main(qint32 argc, char *argv[])
{
    QApplication a(argc, argv);
    MandelbrotMainWindow w;
    w.show();

    return a.exec();
}
